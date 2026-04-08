/*
 * Copyright 2021 Derek Lesho for Codeweavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define VKD3D_DBG_CHANNEL VKD3D_DBG_CHANNEL_API
#include "vkd3d_private.h"

#include <string.h>

/*
 * C-compatible mirror of ExternalHandleShared.h structures.
 * Layout must be kept in sync with the C++ originals.
 */
#define JUICE_EXT_HANDLE_SHARED_MAGIC 0x4A455853u
#define JUICE_MAX_EXTERNAL_HANDLES    512u
#define JUICE_MAX_TEXTURE_METADATA    64u

struct juice_ext_handle_entry
{
    volatile LONG refCount;
    uint32_t type;
    uint64_t localHandle;
    uint64_t handle;
    uint64_t pid;
    uint64_t memoryTypeIndex;
    uint32_t isTimelineSemaphore;
    uint32_t ownerPid;
    volatile LONG hasTextureMetadata;
    uint32_t textureMetadataSize;
    uint8_t  textureMetadata[JUICE_MAX_TEXTURE_METADATA];
};

struct juice_ext_handle_shared_data
{
    uint32_t magic;
    volatile LONG count;
    struct juice_ext_handle_entry entries[JUICE_MAX_EXTERNAL_HANDLES];
};

typedef bool (*PFN_Juice_GetExtHandleSharedMemory)(void **ppSharedData, void **ppMutex);

static struct juice_ext_handle_shared_data *g_sharedData;
static HANDLE g_mutex;
static INIT_ONCE g_initOnce = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK init_juice_shared_memory(INIT_ONCE *once, void *param, void **ctx)
{
    PFN_Juice_GetExtHandleSharedMemory pfn;
    struct juice_ext_handle_shared_data *sd;
    void *pShared = NULL, *pMutex = NULL;
    HMODULE hIcd;

    hIcd = GetModuleHandleA("RemoteGPUVlk.dll");
    if (!hIcd)
    {
        WARN("shared_metadata: RemoteGPUVlk.dll not loaded.\n");
        return TRUE;
    }

    pfn = (PFN_Juice_GetExtHandleSharedMemory)GetProcAddress(hIcd, "Juice_GetExtHandleSharedMemory");
    if (!pfn)
    {
        WARN("shared_metadata: Juice_GetExtHandleSharedMemory not found in ICD.\n");
        return TRUE;
    }

    if (!pfn(&pShared, &pMutex) || !pShared || !pMutex)
    {
        WARN("shared_metadata: ICD returned no shared memory.\n");
        return TRUE;
    }

    sd = (struct juice_ext_handle_shared_data *)pShared;
    if (sd->magic != JUICE_EXT_HANDLE_SHARED_MAGIC)
    {
        WARN("shared_metadata: magic mismatch.\n");
        return TRUE;
    }

    g_sharedData = sd;
    g_mutex = (HANDLE)pMutex;
    INFO("shared_metadata: Juice shared memory acquired from ICD.\n");
    return TRUE;
}

static void ensure_initialized(void)
{
    InitOnceExecuteOnce(&g_initOnce, init_juice_shared_memory, NULL, NULL);
}

bool vkd3d_set_shared_metadata(HANDLE handle, void *buf, uint32_t buf_size)
{
    bool found = false;
    uint32_t i, count;
    uint64_t key;

    ensure_initialized();

    if (!g_sharedData || !g_mutex)
    {
        WARN("vkd3d_set_shared_metadata: Juice shared memory not available.\n");
        return false;
    }

    if (buf_size > JUICE_MAX_TEXTURE_METADATA)
    {
        WARN("vkd3d_set_shared_metadata: metadata too large (%u).\n", buf_size);
        return false;
    }

    key = (uint64_t)(uintptr_t)handle;

    WaitForSingleObject(g_mutex, INFINITE);

    count = (uint32_t)InterlockedOr(&g_sharedData->count, 0);

    for (i = 0; i < count && i < JUICE_MAX_EXTERNAL_HANDLES; ++i)
    {
        struct juice_ext_handle_entry *e = &g_sharedData->entries[i];
        if (InterlockedOr(&e->refCount, 0) > 0 && e->localHandle == key)
        {
            memcpy(e->textureMetadata, buf, buf_size);
            e->textureMetadataSize = buf_size;
            InterlockedExchange(&e->hasTextureMetadata, 1);
            found = true;
            break;
        }
    }

    ReleaseMutex(g_mutex);

    if (found)
        INFO("vkd3d_set_shared_metadata: OK handle=%#"PRIx64" (%u entries).\n", key, count);
    else
        WARN("vkd3d_set_shared_metadata: FAILED handle=%#"PRIx64" not found (%u entries).\n", key, count);

    return found;
}

bool vkd3d_get_shared_metadata(HANDLE handle, void *buf, uint32_t buf_size, uint32_t *metadata_size)
{
    const struct juice_ext_handle_entry *source = NULL;
    uint64_t key, serverHandle = 0;
    bool ok = false, used_fallback = false;
    uint32_t i, count;

    ensure_initialized();

    if (!g_sharedData || !g_mutex)
        return false;

    key = (uint64_t)(uintptr_t)handle;

    WaitForSingleObject(g_mutex, INFINITE);

    count = (uint32_t)InterlockedOr(&g_sharedData->count, 0);

    for (i = 0; i < count && i < JUICE_MAX_EXTERNAL_HANDLES; ++i)
    {
        struct juice_ext_handle_entry *e = &g_sharedData->entries[i];
        if (InterlockedOr(&e->refCount, 0) > 0 && e->localHandle == key)
        {
            serverHandle = e->handle;
            if (InterlockedOr(&e->hasTextureMetadata, 0))
                source = e;
            break;
        }
    }

    if (!source && serverHandle)
    {
        for (i = 0; i < count && i < JUICE_MAX_EXTERNAL_HANDLES; ++i)
        {
            struct juice_ext_handle_entry *e = &g_sharedData->entries[i];
            if (InterlockedOr(&e->refCount, 0) > 0 &&
                e->handle == serverHandle &&
                InterlockedOr(&e->hasTextureMetadata, 0))
            {
                source = e;
                used_fallback = true;
                break;
            }
        }
    }

    if (source)
    {
        uint32_t sz = source->textureMetadataSize;
        if (sz <= buf_size)
        {
            memcpy(buf, (const void *)source->textureMetadata, sz);
            if (metadata_size)
                *metadata_size = sz;
            ok = true;
        }
    }

    ReleaseMutex(g_mutex);

    if (ok)
        INFO("vkd3d_get_shared_metadata: OK handle=%#"PRIx64
             " (server=%#"PRIx64", via %s).\n", key, serverHandle,
             used_fallback ? "server-handle-fallback" : "direct");
    else
        WARN("vkd3d_get_shared_metadata: FAILED handle=%#"PRIx64
             " (server=%#"PRIx64", %u entries).\n", key, serverHandle, count);

    return ok;
}

HANDLE vkd3d_open_kmt_handle(HANDLE kmt_handle)
{
    INFO("vkd3d_open_kmt_handle: passthrough handle=%#"PRIx64".\n",
         (uint64_t)(uintptr_t)kmt_handle);
    return kmt_handle;
}
