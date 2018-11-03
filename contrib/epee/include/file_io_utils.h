// Copyright (c) 2006-2013, Andrey N. Sabelnikov, www.sabelnikov.net
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Andrey N. Sabelnikov nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER  BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


#ifndef _FILE_IO_UTILS_H_
#define _FILE_IO_UTILS_H_

#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#ifdef WIN32
#include <windows.h>
#endif

namespace epee
{
namespace file_io_utils
{
	inline
		bool is_file_exist(const std::string& path)
	{
		boost::filesystem::path p(path);
		return boost::filesystem::exists(p);
	}

	inline
		bool save_string_to_file(const std::string& path_to_file, const std::string& str)
	{
#ifdef WIN32
						WCHAR wide_path[1000];
						int chars = MultiByteToWideChar(CP_UTF8, 0, path_to_file.c_str(), path_to_file.size() + 1, wide_path, 1000);
						if (chars == 0)
								return false;
						HANDLE file_handle = CreateFileW(wide_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
						if (file_handle == INVALID_HANDLE_VALUE)
								return false;
						DWORD bytes_written;
						DWORD bytes_to_write = (DWORD)str.size();
						BOOL result = WriteFile(file_handle, str.data(), bytes_to_write, &bytes_written, NULL);
						CloseHandle(file_handle);
						if (bytes_written != bytes_to_write)
								result = FALSE;
						return result;
#else
		try
		{
			std::ofstream fstream;
			fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fstream.open(path_to_file, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
			fstream << str;
			fstream.close();
			return true;
		}

		catch(...)
		{
			return false;
		}
#endif
	}

	inline
	bool get_file_time(const std::string& path_to_file, time_t& ft)
	{
		boost::system::error_code ec;
		ft = boost::filesystem::last_write_time(boost::filesystem::path(path_to_file), ec);
		if(!ec)
			return true;
		else
			return false;
	}

	inline
		bool set_file_time(const std::string& path_to_file, const time_t& ft)
	{
		boost::system::error_code ec;
		boost::filesystem::last_write_time(boost::filesystem::path(path_to_file), ft, ec);
		if(!ec)
			return true;
		else
			return false;
	}


	inline
		bool load_file_to_string(const std::string& path_to_file, std::string& target_str)
	{
#ifdef WIN32
			WCHAR wide_path[1000];
			int chars = MultiByteToWideChar(CP_UTF8, 0, path_to_file.c_str(), path_to_file.size() + 1, wide_path, 1000);
			if (chars == 0)
				return false;
			HANDLE file_handle = CreateFileW(wide_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file_handle == INVALID_HANDLE_VALUE)
				return false;
			DWORD file_size = GetFileSize(file_handle, NULL);
			if ((file_size == INVALID_FILE_SIZE) || (file_size > 1000000000)) {
				CloseHandle(file_handle);
				return false;
			}
			target_str.resize(file_size);
			DWORD bytes_read;
			BOOL result = ReadFile(file_handle, &target_str[0], file_size, &bytes_read, NULL);
			CloseHandle(file_handle);
			if (bytes_read != file_size)
				result = FALSE;
			return result;
#else
		try
		{
			std::ifstream fstream;
			fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fstream.open(path_to_file, std::ios_base::binary | std::ios_base::in | std::ios::ate);

			std::ifstream::pos_type file_size = fstream.tellg();

			if(file_size > 1000000000)
				return false;//don't go crazy
			size_t file_size_t = static_cast<size_t>(file_size);

			target_str.resize(file_size_t);

			fstream.seekg (0, std::ios::beg);
			fstream.read((char*)target_str.data(), target_str.size());
			fstream.close();
			return true;
		}

		catch(...)
		{
			return false;
		}
#endif
	}

	inline
		bool append_string_to_file(const std::string& path_to_file, const std::string& str)
	{
		try
		{
			std::ofstream fstream;
			fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fstream.open(path_to_file.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::app);
			fstream << str;
			fstream.close();
			return true;
		}

		catch(...)
		{
			return false;
		}
	}

	inline
		bool get_file_size(const std::string& path_to_file, uint64_t &size)
	{
#ifdef WIN32
						WCHAR wide_path[1000];
						int chars = MultiByteToWideChar(CP_UTF8, 0, path_to_file.c_str(), path_to_file.size() + 1, wide_path, 1000);
						if (chars == 0)
								return false;
						HANDLE file_handle = CreateFileW(wide_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
						if (file_handle == INVALID_HANDLE_VALUE)
								return false;
						LARGE_INTEGER file_size;
						BOOL result = GetFileSizeEx(file_handle, &file_size);
						CloseHandle(file_handle);
						if (result) {
								size = file_size.QuadPart;
						}
						return size;
#else
		try
		{
			std::ifstream fstream;
			fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fstream.open(path_to_file, std::ios_base::binary | std::ios_base::in | std::ios::ate);
			size = fstream.tellg();
			fstream.close();
			return true;
		}

		catch(...)
		{
			return false;
		}
#endif		
	}

}
}

#endif //_FILE_IO_UTILS_H_
