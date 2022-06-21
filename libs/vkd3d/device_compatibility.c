/*
 * * Copyright 2022 Juice Technologies, Inc.
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

#include "vkd3d_compatibility.h"

static inline struct d3d12_device *d3d12_device_from_ID3D12CompatibilityDevice(ID3D12CompatibilityDevice *iface)
{
    return CONTAINING_RECORD(iface, struct d3d12_device, ID3D12CompatibilityDevice_iface);
}

ULONG STDMETHODCALLTYPE d3d12_device_compatibility_AddRef(ID3D12CompatibilityDevice *iface)
{
    struct d3d12_device *device = d3d12_device_from_ID3D12CompatibilityDevice(iface);
    return d3d12_device_add_ref(device);
}

static ULONG STDMETHODCALLTYPE d3d12_device_compatibility_Release(ID3D12CompatibilityDevice *iface)
{
    struct d3d12_device *device = d3d12_device_from_ID3D12CompatibilityDevice(iface);
    return d3d12_device_release(device);
}

extern HRESULT STDMETHODCALLTYPE d3d12_device_QueryInterface(d3d12_device_iface *iface,
        REFIID riid, void **object);

static HRESULT STDMETHODCALLTYPE d3d12_device_compatibility_QueryInterface(ID3D12CompatibilityDevice *iface,
        REFIID iid, void **out)
{
    struct d3d12_device *device = d3d12_device_from_ID3D12CompatibilityDevice(iface);
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);
    return d3d12_device_QueryInterface(&device->ID3D12Device_iface, iid, out);
}

HRESULT STDMETHODCALLTYPE d3d12_device_compatibility_CreateSharedResource(
    ID3D12CompatibilityDevice * iface, const D3D12_HEAP_PROPERTIES *pHeapProperties, 
    D3D12_HEAP_FLAGS HeapFlags, const D3D12_RESOURCE_DESC *pDesc, 
    D3D12_RESOURCE_STATES InitialResourceState, const D3D12_CLEAR_VALUE *pOptimizedClearValue,
    const D3D11_RESOURCE_FLAGS *pFlags11, D3D12_COMPATIBILITY_SHARED_FLAGS CompatibilityFlags,
    ID3D12LifetimeTracker *pLifetimeTracker, ID3D12SwapChainAssistant *pOwningSwapchain, REFIID riid,
    void **ppResource)
{
    return E_FAIL;
}

HRESULT STDMETHODCALLTYPE d3d12_device_compatibility_CreateSharedHeap( 
    ID3D12CompatibilityDevice * iface, const D3D12_HEAP_DESC *pHeapDesc, 
    D3D12_COMPATIBILITY_SHARED_FLAGS CompatibilityFlags, REFIID riid, void **ppHeap)
{
    return E_FAIL;
}

HRESULT STDMETHODCALLTYPE d3d12_device_compatibility_ReflectSharedProperties( 
    ID3D12CompatibilityDevice * iface, ID3D12Object *pHeapOrResource, 
    D3D12_REFLECT_SHARED_PROPERTY ReflectType, void *pData, UINT DataSize)
{
    return E_FAIL;
}

CONST_VTBL struct ID3D12CompatibilityDeviceVtbl d3d12_device_compatibility_vtbl =
{
    /* IUnknown methods */
    d3d12_device_compatibility_QueryInterface,
    d3d12_device_compatibility_AddRef,
    d3d12_device_compatibility_Release,

    /* ID3D12CompatibilityDevice methods */
    d3d12_device_compatibility_CreateSharedResource,
    d3d12_device_compatibility_CreateSharedHeap,
    d3d12_device_compatibility_ReflectSharedProperties,
};

