/**
 * This file is part of Pandion.
 *
 * Pandion is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pandion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pandion.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Filename:    ListenThread.cpp
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */
#include "stdafx.h"

#include "Socket.h"
#include "Pile.h"
#include "EventRouter.h"
#include "ListenThread.h"
#include "ServerSession.h"

CListenThread::CListenThread( CPile* p, IStream* pERStream ) : m_pPile( p )
{
	CoGetInterfaceAndReleaseStream( pERStream, IID_IEventRouter, (void **) &m_pER );
}
CListenThread::~CListenThread()
{
}
USHORT CListenThread::AddPort( USHORT port )
{
	Socket* ls = new Socket();
	USHORT lp = ls->Listen( port );
	if( !lp )
	{
		lp = ls->Listen( 0 );
		if( !lp )
			return 0;
	}

	ULONG u = 1;
	ioctlsocket( ls->GetHandle(), FIONBIO, &u );

	m_cs.Lock();
	m_ListenSocks.push_back( ls );
	m_cs.Unlock();

	return lp;
}
void CListenThread::RemPort( USHORT port )
{
	m_cs.Lock();
	for( DWORD i = 0; i < m_ListenSocks.size(); i++ )
	{
		if( m_ListenSocks[i]->GetLocalPort() == port || !port )
		{
			delete m_ListenSocks[i];
			m_ListenSocks.erase( m_ListenSocks.begin()+i );
		}
	}
	m_cs.Unlock();
}
void CListenThread::PreCommand()
{
}
void CListenThread::Command()
{
	if( !m_ListenSocks.size() )
	{
		Sleep( 500 );
		return;
	}

	Socket *pSockRemote;
	CServerSession *pNewSession;
	
	fd_set set;
	FD_ZERO( &set );

	m_cs.Lock();
	for( DWORD i = 0; i < m_ListenSocks.size(); i++ )
		FD_SET( m_ListenSocks[i]->GetHandle(), &set );
	m_cs.Unlock();

	timeval tv = { 0, 500 };
	select( 0, &set, 0, 0, &tv );

	m_cs.Lock();
	for( DWORD i = 0; i < m_ListenSocks.size(); i++ )
		if( FD_ISSET( m_ListenSocks[i]->GetHandle(), &set ) )
		{
			ULONG u = 0;
			pSockRemote = new Socket( accept( m_ListenSocks[i]->GetHandle(), 0, 0 ) );
			ioctlsocket( pSockRemote->GetHandle(), FIONBIO, &u );

			IStream *pERStream = NULL;
			CoMarshalInterThreadInterfaceInStream( IID_IEventRouter, m_pER, &pERStream );
			pNewSession = new CServerSession( m_pPile, pSockRemote, pERStream );
			pNewSession->StartThread();
		}
	m_cs.Unlock();
}
void CListenThread::PostCommand()
{
	RemPort( 0 );
	m_pER->Release();
	delete this;
}