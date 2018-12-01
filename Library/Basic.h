#ifndef LPM_BASIC
#define LPM_BASIC

/************
Classes:
	NotCopyable 
	Error 
	Object
Macros:
	CHECK_ERROR(CONDITION,DESCRIPTION)
************/
namespace ll
{
#ifdef _WIN64
#define LPM_64
#endif

#ifdef LPM_64 
	typedef __int64 vint;
	typedef signed __int64 vsint;
	typedef unsigned __int64 vuint;
#else
	typedef __int32 vint;
	typedef signed __int32 vsint;
	typedef unsigned __int32 vuint;
#endif

typedef signed __int64 pos_t;

#ifdef LPM_64
#define ITOA_S _i64toa_s
#define ITOW_S _i64tow_s
#define I64TOA_S _i64toa_s
#define I64TOW_S _i64tow_s
#define UITOA_S _ui64toa_s
#define UITOW_S _ui64tow_s
#define UI64TOA_S _ui64toa_s
#define UI64TOW_S _ui64tow_s
#else
#define ITOA_S _itoa_s
#define ITOW_S _itow_s
#define I64TOA_S _i64toa_s
#define I64TOW_S _i64tow_s
#define UITOA_S _ui64toa_s
#define UITOW_S _ui64tow_s
#define UI64TOA_S _ui64toa_s
#define UI64TOW_S _ui64tow_s
#endif

#ifndef _MSC_VER
#define override
#endif

/***************
Basic
******************/
class NotCopyable
{
private:
	NotCopyable(const NotCopyable&);
	NotCopyable& operator=(const NotCopyable&);
public:
	NotCopyable();
};

class Error
{
private:
	wchar_t* description;
public:
	Error(wchar_t* _description);
	wchar_t* Description() const;
};

#ifdef _DEBUG
#define CHECK_ERROR(CONDITION, DESCRIPTION) do {if(!(CONDITION)) throw Error(DESCRIPTION);} while(0)
#endif

#ifdef NDEBUG
#define CHECK_ERROR(CONDITION,DESCRIPTION)
#endif

#define CHECK_FAIL(DESCRIPTION) do{throw Error(DESCRIPTION);} while(0)

class Object
{
public:
	virtual ~Object();
};

template<typename T>
class ObjectBox : public Object
{
private:
	T object;
public:
	ObjectBox(const T& _object) : object(_object) { }
	T& UnBox() { return object; }
};

template<typename T, size_t minSize>
union BinaryRetriver
{
	T t;
	char binary[sizeof(T) > minSize ? sizeof(T) : minSize];
};

}

#endif