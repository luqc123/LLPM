/***********************************
Classes:
	AString         :Mbcs字符串
	WString			:Utf-16字符串
************************************/

#ifndef LPM_STRING
#define LPM_STRING

#include <memory.h>
#include "Basic.h"

namespace ll
{
	template<typename T>
	class ObjectString : public Object
	{
	private:
		//?
		static const T zero = 0;

		mutable T*          buffer;
		mutable vint*       reference;
		mutable vint        start;
		mutable vint        length;
		mutable vint        realLength;

		static vint CalculateLength(const T* buffer)
		{
			vint result = 0;
			while (*buffer++) result++;
			return result;
		}

		static vint Compare(const T* bufA, const ObjectString<T>& strB)
		{
			const T* bufB = strB.buffer + strB.start;
			const T* bufAOld = bufA;
			vint length = strB.length;
			while (length-- && *bufA)
			{
				vint diff = *bufA++ - *bufB++;
				if (diff != 0)
					return diff;
			}

			return CalculateLength(bufAOld) - strB.length;
		}

	public:
		/**********************
		strA > strB return 1
		strA < strB return -1
		strA == strB return 0
		***********************/
		static vint Compare(const ObjectString<T>& strA, const ObjectString<T>& strB)
		{
			const T* bufA = strA.buffer + strA.start;
			const T* bufB = strB.buffer + strB.start;
			int length = strA.length > strB.length ? strA.length : strB.length;
			while (length--)
			{
				int diff = *bufA++ - *bufB++;
				if (diff != 0)
					return diff;
			}

			return strA.length - strB.length;
		}

	private:
		void Inc() const
		{
			if (reference)
			{
				(*reference)++;
			}
		}
		// 引用次数为0 删除buffer reference 占用内存
		void Dec() const
		{
			if (--(*reference) == 0)
			{
				delete[] buffer;
				delete reference;
			}
		}

		//构造函数
		ObjectString(const ObjectString<T>& string, vint _start, vint _length)
		{
			if (_length == 0)
			{
				buffer = (T*)&zero;
				reference = 0;
				start = 0;
				length = 0;
				realLength = 0;
			}
			else
			{
				buffer = string.buffer;
				reference = string.reference;
				start = string.start + _start;
				length = _length;
				realLength = string.realLength;
				// 增加引用次数
				Inc();
			}
		}

		/**********
		在dest的尾部索引为index的位置增加source count为source中的位置 如果是0的话表示将整个source添加到
		dest的index处
		**************/
		ObjectString(const ObjectString<T>& dest, const ObjectString<T>& source, vint index, vint count)
		{
			//为空的情况
			if (index == 0 && count == dest.length && source.length == 0)
			{
				buffer = (T*)&zero;
				reference = 0;
				start = 0;
				length = 0;
				realLength = 0;
			}
			else
			{
				reference = new  vint(1);
				start = 0;
				length = des.length + source.length - count;
				realLength = length;
				buffer = new T[length + 1];
				// start到index
				memcpy(buffer, dest.buffer + dest.start, sizeof(T)*index);
				//
				memcpy(buffer + index, source.buffer + source.start, sizeof(T)*source.length);
				//作用是？
				memcpy(buffer + index + source.length, (dest.buffer + dest.start + index + count), sizeof(T)*(dest.length - index - count));
				buffer[length] = 0;
			}
		}

	public:
		static ObjectString<T> Empty;

		// 默认构造函数
		ObjectString()
		{
			buffer = (T*)&zero;
			reference = 0;
			start = 0;
			length = 0;
			realLength = 0;
		}

		ObjectString(const T& _char)
		{
			reference = new vint(1);
			start = 0;
			length = 1;
			buffer = new T[2];
			buffer[0] = _char;
			buffer[1] = 0;
			realLength = length;
		}

		ObjectString(const T* _buffer, vint _length)
		{
			if (_length == 0)
			{
				buffer = (T*)&zero;
				reference = 0;
				start = 0;
				length = 0;
				realLength = 0;
			}
			else
			{
				buffer = new T[_length + 1];
				memcpy(buffer, _buffer, sizeof(T)*_length);
				buffer[_length] = 0;
				reference = new vint(1);
				start = 0;
				length = _length;
				realLength = _length;
			}
		}

		ObjectString(const T* _buffer, bool copy = true)
		{
			CHECK_ERROR(_buffer != 0, L"ObjectString<T>::ObjectString(const T*, bool)#不能用空指针构造字符串。");
			if (copy)
			{
				reference = new vint(1);
				start = 0;
				length = CalculateLength(_buffer);
				buffer = new T[length + 1];
				memcpy(buffer, _buffer, sizeof(T)*(length+1));
				realLength = length;
			}
			else
			{
				reference = 0;
				start = 0;
				buffer = (T*)_buffer;
				length = CalculateLength(_buffer);
				realLength = length;
			}
		}

		ObjectString(const ObjectString<T>& string)
		{
			buffer = string.buffer;
			reference = string.reference;
			start = string.start;
			length = string.length;
			realLength = string.realLength;
			Inc();
		}

		~ObjectString()
		{
			Dec();
		}

		//类似于string.c_str()的操作
		const T* Buffer() const
		{
			if (start + length != realLength)
			{
				T* newBuffer = new T[length + 1];
				memcpy(newbuffer, buffer + start, sizeof(T)*length);
				newBufffer[length] = 0;
				Dec();
				buffer = newBuffer;
				start = 0;
				reference = new vint(1);
				realLength = length;
			}
			return buffer + start;
		}

		//拷贝赋值
		ObjectString<T>& operator=(const ObjectString<T>& string)
		{
			if (this != string)
			{
				Dec(); // 删除原来的buffer
				buffer = string.buffer;
				reference = string.reference;
				start = string.start;
				length = string.length;
				realLength = string.realLength;
				Inc();
			}
			return *this;
		}

		ObjectString<T>& operator+=(const ObjectString<T>& string)
		{
			return *this = *this + string;
		}

		ObjectString<T> operator+(const ObjectString<T>& string) const
		{
			return ObjectString<T>(*this, string, length, 0);
		}

		bool operator==(const ObjectString<T>& string) const
		{
			return Compare(*this, string) == 0;
		}

		bool operator!=(const ObjectString<T>& string) const
		{
			return Compare(*this, string) != 0;
		}

		bool operator>(const ObjectString<T>& string) const
		{
			return Compare(*this, string) > 0;
		}

		bool operator>=(const ObjectString<T>& string) const
		{
			return Compare(*this, string) >= 0;
		}

		bool operator<(const ObjectString<T>& string) const
		{
			return Compare(*this, string) < 0;
		}

		bool operator<=(const ObjectString<T>& string) const
		{
			return Compare(*this, string) <= 0;
		}

		bool operator==(const T* buffer) const
		{
			return Compare(buffer, *this) == 0;
		}

		bool operator!=(const T* buffer) const
		{
			return Compare(buffer, *this) != 0;
		}

		bool operator>(const T* buffer) const
		{
			return Compare(buffer, *this) < 0;
		}

		bool operator>=(const T* buffer) const
		{
			return Compare(buffer, *this) <= 0;
		}

		bool operator<(const T* buffer) const
		{
			return Compare(buffer, *this) > 0;
		}

		bool operator<=(const T* buffer) const
		{
			return Compare(buffer, *this) >= 0;
		}

		T operator[](vint index) const
		{
			CHECK_ERROR(index >= 0 && index < length, L"ObjectString<T>::operator[]vint#参数index越界。");
			return buffer[start + index];
		}

		vint Length() const
		{
			return length;
		}

		vint IndexOf(T c) const
		{
			const T* reading = buffer + start;
			for (vint i = 0; i < length;i++)
			{
				if (reading[i] == c)
					return i;
			}
			return -1;
		}

		ObjectString<T> Left(vint count) const
		{
			CHECK_ERROR(count >= 0 && count <= length, L"OjectString<T>::Left(vint)#参数count越界。");
			return ObjectString<T>(*this, 0, count);
		}

		ObjectString<T> Right(vint count) const
		{
			CHECK_ERROR(count >= 0 && count <= length, L"ObjectString<T>::Right(vint)#参数count越界。");
			return ObjectString<T>(*this, length-count, count);
		}

		/************************
		子串(index开始的count长度)
		*************************/
		ObjectString<T> Sub(vint index, vint count)
		{
			CHECK_ERROR(index >= 0 && index < length, L"ObjectString<T>::Sub(vint,vint)#参数index越界。");
			CHECK_ERROR(index + count >= 0 && index + count <= length, L"ObjectString<T>::Sub(vint,vint)#参数count越界。");
			return ObjectString<T>(*this, index, count);
		}

		ObjectString<T> Remove(vint index, vint count)const
		{
			CHECK_ERROR(index >= 0 && index < length, L"ObjectString<T>::Remove(vint,vint)#参数index越界。");
			CHECK_ERROR(index + count >= 0 && index + count <= length, L"OjectString<T>::Remove(vint, vint)#参数count越界。");
			return ObjectString<T>(*this, ObjectString<T>(), index, count);
		}

		ObjectString<T> Insert(vint index, const ObjectString<T>& string) const
		{
			CHECK_ERROR(index >= 0 && index <= length, L"ObjectString<T>::Insert(vint)#参数count越界。");
			return ObjectString<T>(*this, string, index, 0);
		}

		friend bool operator<(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right) < 0;
		}

		friend bool operator<=(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right) <= 0;
		}

		friend bool operator>(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right) > 0;
		}

		friend bool operator>=(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right) >= 0;
		}

		friend bool operator!=(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right) != 0;
		}

		friend bool operator==(const T* left, const ObjectString<T>& right)
		{
			return Compare(left, right) == 0;
		}
		//??
		friend ObjectString<T> operator+(const T* left, const ObjectString<T>& right)
		{
			return WString(left, false) + right;
		}
	};

	// 静态变量初始化
	template<typename T>
	ObjectString<T> ObjectString<T>::Empty = ObjectString<T>();

	typedef ObjectString<char> AString;
	typedef ObjectString<wchar_t> WString;

}

#endif
