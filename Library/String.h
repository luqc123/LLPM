/***********************************
Classes:
	AString         :Mbcs�ַ���
	WString			:Utf-16�ַ���
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
		// ���ô���Ϊ0 ɾ��buffer reference ռ���ڴ�
		void Dec() const
		{
			if (--(*reference) == 0)
			{
				delete[] buffer;
				delete reference;
			}
		}

		//���캯��
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
				// �������ô���
				Inc();
			}
		}

		/**********
		��dest��β������Ϊindex��λ������source countΪsource�е�λ�� �����0�Ļ���ʾ������source��ӵ�
		dest��index��
		**************/
		ObjectString(const ObjectString<T>& dest, const ObjectString<T>& source, vint index, vint count)
		{
			//Ϊ�յ����
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
				// start��index
				memcpy(buffer, dest.buffer + dest.start, sizeof(T)*index);
				//
				memcpy(buffer + index, source.buffer + source.start, sizeof(T)*source.length);
				//�����ǣ�
				memcpy(buffer + index + source.length, (dest.buffer + dest.start + index + count), sizeof(T)*(dest.length - index - count));
				buffer[length] = 0;
			}
		}

	public:
		static ObjectString<T> Empty;

		// Ĭ�Ϲ��캯��
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
			CHECK_ERROR(_buffer != 0, L"ObjectString<T>::ObjectString(const T*, bool)#�����ÿ�ָ�빹���ַ�����");
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

		//������string.c_str()�Ĳ���
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

		//������ֵ
		ObjectString<T>& operator=(const ObjectString<T>& string)
		{
			if (this != string)
			{
				Dec(); // ɾ��ԭ����buffer
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
			CHECK_ERROR(index >= 0 && index < length, L"ObjectString<T>::operator[]vint#����indexԽ�硣");
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
			CHECK_ERROR(count >= 0 && count <= length, L"OjectString<T>::Left(vint)#����countԽ�硣");
			return ObjectString<T>(*this, 0, count);
		}

		ObjectString<T> Right(vint count) const
		{
			CHECK_ERROR(count >= 0 && count <= length, L"ObjectString<T>::Right(vint)#����countԽ�硣");
			return ObjectString<T>(*this, length-count, count);
		}

		/************************
		�Ӵ�(index��ʼ��count����)
		*************************/
		ObjectString<T> Sub(vint index, vint count)
		{
			CHECK_ERROR(index >= 0 && index < length, L"ObjectString<T>::Sub(vint,vint)#����indexԽ�硣");
			CHECK_ERROR(index + count >= 0 && index + count <= length, L"ObjectString<T>::Sub(vint,vint)#����countԽ�硣");
			return ObjectString<T>(*this, index, count);
		}

		ObjectString<T> Remove(vint index, vint count)const
		{
			CHECK_ERROR(index >= 0 && index < length, L"ObjectString<T>::Remove(vint,vint)#����indexԽ�硣");
			CHECK_ERROR(index + count >= 0 && index + count <= length, L"OjectString<T>::Remove(vint, vint)#����countԽ�硣");
			return ObjectString<T>(*this, ObjectString<T>(), index, count);
		}

		ObjectString<T> Insert(vint index, const ObjectString<T>& string) const
		{
			CHECK_ERROR(index >= 0 && index <= length, L"ObjectString<T>::Insert(vint)#����countԽ�硣");
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

	// ��̬������ʼ��
	template<typename T>
	ObjectString<T> ObjectString<T>::Empty = ObjectString<T>();

	typedef ObjectString<char> AString;
	typedef ObjectString<wchar_t> WString;

}

#endif
