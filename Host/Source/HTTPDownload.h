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
 * Filename:    HTTPDownload.h
 * Author(s):   Dries Staelens
 * Copyright:   Copyright (c) 2009 Dries Staelens
 * Description: TODOTODOTODO
 */

#pragma once

class HTTPEventRouter;

class HTTPDownload
{
private:
	HTTPEventRouter* m_EventRouter;
	DWORD            m_SessionID;

	HINTERNET    m_hInternet;
	DWORD        m_Progress;
	DWORD        m_Length;
	std::wstring m_URI;
	std::wstring m_FileName;
	std::wstring m_Address;
	USHORT       m_Port;
	bool         m_Abort;
public:
	HTTPDownload(HINTERNET hInternet, HTTPEventRouter* eventRouter, DWORD sessionID);
	~HTTPDownload();

	void Get(std::wstring filename, std::wstring URI, DWORD offset, DWORD len,
		 std::wstring address, USHORT port);
	void Abort();
	DWORD GetProgress();
	DWORD GetLength();
private:
	static DWORD __stdcall DownloadFileProc(void *pThis);
	DWORD DownloadFile();
};
