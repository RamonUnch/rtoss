#ifndef _KILIB_STRING_H_
#define _KILIB_STRING_H_
#include "types.h"
#include "memory.h"
#include "ktlaptr.h"
#ifndef __ccdoc__
namespace ki {
#endif
#ifdef _UNICODE
	#define XTCHAR char
#else
	#define XTCHAR wchar_t
#endif



//=========================================================================
//@{ @pkg ki.StdLib //@}
//@{
//	文字列処理
//
//	かなりMFCのCStringをパクってます。とりあえず operator= による
//	単純代入にはほとんどコストがかからないようにしました。SubStr()の
//	時もコピーしないようにしようかとも思ったんですが、そこまでは
//	要らないだろうという気もするので…。
//@}
//=========================================================================

class String : public Object
{
public:

	//@{ 空文字列作成 //@}
	String();
	~String();

	//@{ 別のStringのコピー //@}
	String( const String& obj );

	//@{ 別の文字配列のコピー //@}
	String( const TCHAR* str, long siz=-1 );

	//@{ リソースから作成 //@}
	explicit String( UINT rsrcID );

	//@{ 大文字小文字を区別する比較 //@}
	bool operator==( LPCTSTR s ) const;
	bool operator==( const String& obj ) const;

	//@{ 大文字小文字を区別しない比較 //@}
	bool isSame( LPCTSTR s ) const;
	bool isSame( const String& obj ) const;

	//@{ 単純代入 //@}
	String& operator=( const String& obj );
	String& operator=( const TCHAR* s );
	String& operator=( const XTCHAR* s );

	//@{ 加算代入 //@}
	String& operator+=( const String& obj );
	String& operator+=( const TCHAR* s );
	String& operator+=( TCHAR c );

	//@{ リソースロード //@}
	String& Load( UINT rsrcID );

	//@{ 右を削る //@}
	void TrimRight( ulong siz );

	//@{ intから文字列へ変換 //@}
	String& SetInt( int n );

	//@{ 文字列からintへ変換 //@}
	int GetInt();

public:

	//@{ 文字列バッファを返す //@}
	const TCHAR* c_str() const;

	//@{ 長さ //@}
	ulong len() const;

	//@{ 要素 //@}
	const TCHAR operator[](int n) const;

	//@{ ワイド文字列に変換して返す //@}
	const wchar_t* ConvToWChar() const;

	//@{ ConvToWCharの返値バッファの解放 //@}
	void FreeWCMem( const wchar_t* wc ) const;

public:

	//@{ 次の一文字 //@}
	static TCHAR*       next( TCHAR* p );
	static const TCHAR* next( const TCHAR* p );

	//@{ ２バイト文字の先頭かどうか？ //@}
	static bool isLB( TCHAR c );

	//@{ 文字列からintへ変換 //@}
	static int GetInt( const TCHAR* p );

protected:

	// 書き込み可能なバッファを、終端含めて最低でもminimum文字分用意する
	TCHAR* AllocMem( ulong minimum );
	TCHAR* ReallocMem( ulong minimum );

	// 書き込み終了後、長さを再設定
	void UnlockMem( long siz=-1 );

private:

	struct StringData
	{
		long  ref;         // 参照カウンタ
		ulong len;         // 終端'\0'を含める長さ
		ulong alen;        // 割り当てられているメモリのサイズ
		TCHAR* buf() const // TCHAR buf[alen]
			{ return reinterpret_cast<TCHAR*>(
				const_cast<StringData*>(this+1)
			); }
	};

private:

	TCHAR*  AllocMemHelper( ulong minimum, const TCHAR* str, ulong siz );
	String& CatString( const TCHAR* str, ulong siz );
	String& SetString( const TCHAR* str, ulong siz );
	void    SetData( StringData* d );
	void    ReleaseData();
	static  StringData* null();
	        StringData* data() const;

private:

	StringData*        data_;
	static StringData* nullData_;
	static char        lb_[256];

private:

	static void LibInit();
	friend void APIENTRY Startup();
};



//-------------------------------------------------------------------------
#ifndef __ccdoc__

// 初期化
inline String::String()
	{ SetData( null() ); }
// 初期化
inline String::String( UINT rsrcID )
	{ SetData( null() ), Load( rsrcID ); }
// 初期化
inline String::String( const String& obj )
	{ SetData( obj.data() ); }

// ポインタ計算サポート
#if !defined(_UNICODE) && defined(_MBCS)
	inline TCHAR* String::next( TCHAR* p )
		{ return p + lb_[*(uchar*)p]; }
	inline const TCHAR* String::next( const TCHAR* p )
		{ return p + lb_[*(const uchar*)p]; }
	inline bool String::isLB( TCHAR c )
		{ return lb_[(uchar)c]==2; }
#else // _UNICODE or _SBCS
	inline TCHAR* String::next( TCHAR* p )
		{ return p + 1; }
	inline const TCHAR* String::next( const TCHAR* p )
		{ return p + 1; }
	inline bool String::isLB( TCHAR c )
		{ return false; }
#endif

// 内部メモリ確保
inline TCHAR* String::AllocMem( ulong minimum )
	{ return AllocMemHelper( minimum, TEXT(""), 1 ); }
// 内部メモリ固定
inline void String::UnlockMem( long siz )
	{ data_->len = 1 + (siz==-1 ? ::lstrlen(c_str()) : siz); }

// ０文字データ
inline String::StringData* String::null()
	{ return nullData_; }
// 内部データ構造
inline String::StringData* String::data() const
	{ return data_; }
// 初期化
inline void String::SetData( String::StringData* d )
	{ data_=d, data_->ref++; }

// 属性
inline const TCHAR* String::c_str() const
	{ return data_->buf(); }
// 属性
inline ulong String::len() const
	{ return data_->len-1; }
// 要素
inline const TCHAR String::operator[](int n) const
	{ return data_->buf()[n]; }

// 比較
inline bool String::operator==( LPCTSTR s ) const
	{ return 0==::lstrcmp( c_str(), s ); }
// 比較
inline bool String::operator==( const String& obj ) const
	{ return (data_==obj.data_ ? true : operator==( obj.c_str() )); }
// 比較
inline bool String::isSame( LPCTSTR s ) const
	{ return 0==::lstrcmpi( c_str(), s ); }
// 比較
inline bool String::isSame( const String& obj ) const
	{ return (data_==obj.data_ ? true : operator==( obj.c_str() )); }

// 要コピー代入
inline String& String::operator = ( const TCHAR* s )
	{ return SetString( s, ::lstrlen(s) ); }
// 合成
inline String& String::operator += ( const String& obj )
	{ return CatString( obj.c_str(), obj.len() ); }
// 合成
inline String& String::operator += ( const TCHAR* s )
	{ return CatString( s, ::lstrlen(s) ); }
// 合成
inline String& String::operator += ( TCHAR c )
	{ return CatString( &c, 1 ); }

// 変換
inline int String::GetInt()
	{ return GetInt( data_->buf() ); }

//@{ String + String //@}
inline const String operator+( const String& a, const String& b )
	{ return String(a) += b; }
//@{ String + TCHAR* //@}
inline const String operator+( const String& a, const TCHAR* b )
	{ return String(a) += b; }
//@{ TCHAR* + String //@}
inline const String operator+( const TCHAR* a, const String& b )
	{ return String(a) += b; }

// ConvToWCharの返値バッファの解放
inline void String::FreeWCMem( const wchar_t* wc ) const
#ifdef _UNICODE
	{}
#else // _MBCS or _SBCS
	{ delete [] const_cast<wchar_t*>(wc); }
#endif



#endif // __ccdoc__
#undef XTCHAR
//=========================================================================
//@{
//	文字列処理＋α
//
//	Stringクラス内のバッファ確保関数を呼べるようにした版Stringです。
//@}
//=========================================================================

struct RawString : public String
{
	TCHAR* AllocMem( ulong m ) { return String::AllocMem(m); }
	void UnlockMem()           { String::UnlockMem(); }
};

}      // namespace ki


//=========================================================================
//@{
//	文字列処理＋α２
//
//	Wide文字版関数を自前で
//@}
//=========================================================================

#ifdef _UNICODE
	#define my_lstrlenW ::lstrlenW
	#define my_lstrcpyW ::lstrcpyW
#else
	inline static
	unicode* my_lstrcpyW( unicode* const d, const unicode* s )
	{
		for(unicode* n=d; *n++=*s++;);
		return d;
	}

	inline static
	int my_lstrlenW( const unicode* const d )
	{
		const unicode* n;
		for(n=d; *n; ++n);
		return static_cast<int>(n-d);
	}
#endif

#if defined(WIN32S) || (!defined(_UNICODE) && defined(TARGET_VER) && TARGET_VER < 350)
#define WideCharToMultiByte SimpleWC2MB
#define MultiByteToWideChar SimpleMB2WC

int WINAPI SimpleWC2MB_init(UINT cp, DWORD flg, LPCWSTR s, int sl, LPSTR d, int dl, LPCSTR defc, LPBOOL useddef);
static int (WINAPI *SimpleWC2MB)(UINT cp, DWORD flg, LPCWSTR s, int sl, LPSTR d, int dl, LPCSTR defc, LPBOOL useddef) = SimpleWC2MB_init;
int WINAPI SimpleWC2MB_fallback(UINT cp, DWORD flg, LPCWSTR s, int sl, LPSTR d, int dl, LPCSTR defc, LPBOOL useddef);

int WINAPI SimpleMB2WC_init(UINT cp, DWORD flg, LPCSTR s, int sl, LPWSTR d, int dl);
static int(WINAPI*SimpleMB2WC)(UINT cp, DWORD flg, LPCSTR s, int sl, LPWSTR d, int dl) = SimpleMB2WC_init;
int WINAPI SimpleMB2WC_fallback(UINT cp, DWORD flg, LPCSTR s, int sl, LPWSTR d, int dl);
#endif

//=========================================================================

#endif // _KILIB_STRING_H_
