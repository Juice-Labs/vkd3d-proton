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

static inline struct d3d12_command_queue *d3d12_command_queue_from_ID3D12CommandQueueInternal(ID3D12CommandQueueInternal *iface)
{
    return CONTAINING_RECORD(iface, struct d3d12_command_queue, ID3D12CommandQueueInternal_iface);
}

extern ULONG STDMETHODCALLTYPE d3d12_command_queue_AddRef(ID3D12CommandQueue *iface);

ULONG STDMETHODCALLTYPE d3d12_command_queue_internal_AddRef(ID3D12CommandQueueInternal *iface)
{
    struct d3d12_command_queue *command_queue = d3d12_command_queue_from_ID3D12CommandQueueInternal(iface);
    return d3d12_command_queue_AddRef(&command_queue->ID3D12CommandQueue_iface);
}

extern ULONG STDMETHODCALLTYPE d3d12_command_queue_Release(ID3D12CommandQueue *iface);

static ULONG STDMETHODCALLTYPE d3d12_command_queue_internal_Release(ID3D12CommandQueueInternal *iface)
{
    struct d3d12_command_queue *command_queue = d3d12_command_queue_from_ID3D12CommandQueueInternal(iface);
    return d3d12_command_queue_Release(&command_queue->ID3D12CommandQueue_iface);
}

extern HRESULT STDMETHODCALLTYPE d3d12_command_queue_QueryInterface(ID3D12CommandQueue *iface,
        REFIID riid, void **object);

static HRESULT STDMETHODCALLTYPE d3d12_command_queue_internal_QueryInterface(ID3D12CommandQueueInternal *iface,
        REFIID iid, void **out)
{
    struct d3d12_command_queue *command_queue = d3d12_command_queue_from_ID3D12CommandQueueInternal(iface);
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);
    return d3d12_command_queue_QueryInterface(&command_queue->ID3D12CommandQueue_iface, iid, out);
}

static HRESULT STDMETHODCALLTYPE d3d12_command_queue_internal_Command1(ID3D12CommandQueueInternal *iface)
{
    return E_FAIL;
}

CONST_VTBL struct ID3D12CommandQueueInternalVtbl d3d12_command_queue_internal_vtbl =
{
    /* IUnknown methods */
    d3d12_command_queue_internal_QueryInterface,
    d3d12_command_queue_internal_AddRef,
    d3d12_command_queue_internal_Release,

    /* ID3D12CommandQueueInternal methods */
    d3d12_command_queue_internal_Command1
};

