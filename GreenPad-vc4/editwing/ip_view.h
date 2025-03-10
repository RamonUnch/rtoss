#ifndef _EDITWING_IP_VIEW_H_
#define _EDITWING_IP_VIEW_H_
#include "ewView.h"
#include "ip_doc.h"
using namespace ki;
#ifndef __ccdoc__
namespace editwing {
namespace view {
#endif



using doc::DocImpl;



//=========================================================================
//@{ @pkg editwing.View.Impl //@}
//@{
//	描画基本ルーチン
//
//	利用するには、Canvasオブジェクトから getPainter して使います。
//	画面用デバイスコンテキストのレイヤです。きちんと書いておけば印刷機能を
//	追加するときに楽なのかもしれませんが、そんなことを考える計画性が
//	あるはずもなく極めて適当に…。
//@}
//=========================================================================

class Painter : public Object
{
public:

	~Painter();

	//@{ 指定位置に一文字出力 //@}
	void CharOut( unicode ch, int x, int y );

	//@{ 指定位置に文字列を出力 //@}
	void StringOut( const unicode* str, int len, int x, int y );

	//@{ 文字色切り替え //@}
	void SetColor( int i );

	//@{ 背景色で塗りつぶし //@}
	void Fill( const RECT& rc );

	//@{ 反転 //@}
	void Invert( const RECT& rc );

	//@{ 線を引く //@}
	void DrawLine( int x1, int y1, int x2, int y2 );

	//@{ クリップ領域設定 //@}
	void SetClip( const RECT& rc );

	//@{ クリップ領域解除 //@}
	void ClearClip();

	//@{ 半角スペース用記号描画 //@}
	void DrawHSP( int x, int y, int times );

	//@{ 全角スペース用記号描画 //@}
	void DrawZSP( int x, int y, int times );

	void SetupDC(HDC hdc);
	void RestoreDC();

public:

	//@{ 高さ(pixel) //@}
	int H() const { return height_; }

	//@{ 数字幅(pixel) //@}
	int F() const { return figWidth_; }

	//@{ 文字幅(pixel) //@}
	int Wc( unicode ch ) const
		{
			if( widthTable_[ ch ] == -1 )
#ifdef WIN32S
				if(ch > 0x100)
				{
					::GetCharWidthA( cdc_, 'x', 'x', widthTable_+ch );
					widthTable_[ ch ] *= 2;
				}
				else
				{
					::GetCharWidthA( cdc_, ch, ch, widthTable_+ch );
				}
#else
				::GetCharWidthW( cdc_, ch, ch, widthTable_+ch );
#endif
			return widthTable_[ ch ];
		}
	int W( const unicode* pch ) const // 1.08 サロゲートペア回避
		{
			unicode ch = *pch;
			if( widthTable_[ ch ] == -1 )
			{
				if( isHighSurrogate(ch) )
				{
					SIZE sz;
					if( ::GetTextExtentPoint32W( cdc_, pch, 2, &sz ) )
						return sz.cx;
					int w = 0;
#ifdef WIN32S
					if(ch > 0x100)
					{
						::GetCharWidthA( cdc_, 'x', 'x', &w );
						w *= 2;
					}
					else
					{
						::GetCharWidthA( cdc_, ch, ch, &w );
					}
#else
					::GetCharWidthW( cdc_, ch, ch, &w );
#endif
					return w;
				}
#ifdef WIN32S
				if(ch > 0x100)
				{
					::GetCharWidthA( cdc_, 'x', 'x', widthTable_+ch );
					widthTable_[ ch ] *= 2;
				}
				else
				{
					::GetCharWidthA( cdc_, ch, ch, widthTable_+ch );
				}
#else
				::GetCharWidthW( cdc_, ch, ch, widthTable_+ch );
#endif
			}
			return widthTable_[ ch ];
		}

	//@{ 標準文字幅(pixel) //@}
	int W() const { return widthTable_[ L'x' ]; }

	//@{ 次のタブ揃え位置を計算 //@}
	//int nextTab(int x) const { int t=T(); return (x/t+1)*t; }
	int nextTab(int x) const { int t=T(); return ((x+4)/t+1)*t; }
	private: int T() const { return widthTable_[ L'\t' ]; } public:

	//@{ 現在のフォント情報 //@}
	const LOGFONT& LogFont() const { return logfont_; }

	//@{ 特別文字を描画するか否か //@}
	bool sc( int i ) const { return scDraw_[i]; }

private:

	const HWND   hwnd_;// Window in which we paint
	HDC          dc_;  // Device context used for Painting (non const)
	const HDC    cdc_; // Compatible DC used for W() (const)
	const HFONT  font_;
	const HPEN   pen_;
	const HBRUSH brush_;
	HFONT  oldfont_;   // Old objects to be released before
	HPEN   oldpen_;    // the EndPaint() call.
	HBRUSH oldbrush_;  //
	int          height_;
	int*         widthTable_;
	int          figWidth_;
	LOGFONT      logfont_;
	COLORREF     colorTable_[7];
	bool         scDraw_[5];

private:

	Painter( HWND hwnd, const VConfig& vc );
	HWND getWHND() { return hwnd_; }
	friend class Canvas;
	NOCOPY(Painter);
};



//=========================================================================
//@{
//	描画可能領域
//
//	ウインドウサイズの変更や折り返しの有無やフォントの設定などに
//	対応して、描画領域のサイズを適当に管理します。やることは
//	とりあえずそれだけ。
//@}
//=========================================================================

class Canvas : public Object
{
public:

	Canvas( const View& vw );

	//@{ Viewの大きさ変更イベント処理
	//	 @return 折り返し幅が変わったらtrue //@}
	bool on_view_resize( int cx, int cy );

	//@{ 行数変更イベント処理
	//	 @return テキスト領域の幅が変わったらtrue //@}
	bool on_tln_change( ulong tln );

	//@{ フォント変更イベント処理 //@}
	void on_font_change( const VConfig& vc );

	//@{ 設定変更イベント処理 //@}
	void on_config_change( int wrap, bool showln );

public:

	//@{ [行番号を表示するか否か] //@}
	bool showLN() const { return showLN_; }

	//@{ [-1:折り返し無し  0:窓右端  else:指定文字数] //@}
	int wrapType() const { return wrapType_; }

	//@{ 折り返し幅(pixel) //@}
	ulong wrapWidth() const { return wrapWidth_; }

	//@{ 表示領域の位置(pixel) //@}
	const RECT& zone() const { return txtZone_; }

	//@{ 描画用オブジェクト //@}
	Painter& getPainter() const { return *font_; }

private:

	int  wrapType_;  // [ -1:折り返し無し  0:窓右端  else:指定文字数 ]
	bool showLN_;    // [ 行番号を表示するか否か ]

	dptr<Painter> font_; // 描画用オブジェクト
	ulong    wrapWidth_; // 折り返し幅(pixel)
	RECT       txtZone_; // テキスト表示域の位置(pixel)
	int         figNum_; // 行番号の桁数

private:

	bool CalcLNAreaWidth();
	void CalcWrapWidth();

private:

	NOCOPY(Canvas);
};



//=========================================================================
//@{
//	行毎の折り返し情報
//@}
//=========================================================================

struct WLine : public storage<ulong>
{
	// [0]   : その行の折り返し無しでの横幅を格納
	// [1-n] : n行目の終端のindexを格納。
	//
	//   例えば "aaabbb" という論理行を "aaab" "bb" と折るなら
	//   {48, 4, 6} などという長さ３の配列となる。

	WLine() : storage<ulong>(2) {}
	ulong& width()      { return (*this)[0]; }
	ulong width() const { return (*this)[0]; }
	ulong rln() const   { return size()-1; }
};



//=========================================================================
//@{
//	再描画範囲を指定するためのフラグ
//@}
//=========================================================================

enum ReDrawType
{
	LNAREA, // 行番号ゾーンのみ
	LINE,   // 変更のあった一行のみ
	AFTER,  // 変更のあった行以下全部
	ALL     // 全画面
};



//=========================================================================
//@{
//	描画処理を細かく指定する構造体
//@}
//=========================================================================

struct VDrawInfo
{
	const RECT rc;  // 再描画範囲
	int XBASE;      // 一番左の文字のx座標
	int XMIN;       // テキスト再描画範囲左端
	int XMAX;       // テキスト再描画範囲右端
	int YMIN;       // テキスト再描画範囲上端
	int YMAX;       // テキスト再描画範囲下端
	ulong TLMIN;    // テキスト再描画範囲上端論理行番号
	int SXB, SXE;   // 選択範囲のx座標
	int SYB, SYE;   // 選択範囲のy座標

	explicit VDrawInfo( const RECT& r ) : rc(r) {}
};



//=========================================================================
//@{
//	折り返しedテキストの管理・表示等
//
//	Canvasクラスによって計算された領域サイズを参考に、テキストの
//	折り返し処理を実行する。ここで、スクロール制御、描画処理など
//	主要な処理は全て実行することになる。
//@}
//=========================================================================

class ViewImpl : public Object
{
public:

	ViewImpl( View& vw, DocImpl& dc );

	//@{ 折り返し方式切替 //@}
	void SetWrapType( int wt );

	//@{ 行番号表示/非表示切替 //@}
	void ShowLineNo( bool show );

	//@{ 表示色・フォント切替 //@}
	void SetFont( const VConfig& vc );

		//@{ テキスト領域のサイズ変更イベント //@}
		void on_view_resize( int cx, int cy );

	void DoResize( bool wrapWidthChanged );
	void DoConfigChange();

	//@{ テキストデータの更新イベント //@}
	void on_text_update( const DPos& s,
		const DPos& e, const DPos& e2, bool bAft, bool mCur );

	//@{ 描画処理 //@}
	void on_paint( const PAINTSTRUCT& ps );

public:

	//@{ 全表示行数 //@}
	ulong vln() const { return vlNum_; }

	//@{ 一行の表示行数 //@}
	ulong rln( ulong tl ) const { return wrap_[tl].rln(); }

	//@{ 折り返し位置 //@}
	ulong rlend( ulong tl, ulong rl ) const { return wrap_[tl][rl+1]; }

	//@{ 一個でも折り返しが存在するか否か //@}
	bool wrapexists() const { return doc_.tln() != vln(); }

	//@{ カーソル //@}
	Cursor& cur() { return cur_; }

	//@{ フォント //@}
	const Painter& fnt() const { return cvs_.getPainter(); }


	void on_hscroll( int code, int pos );
	void on_vscroll( int code, int pos );
	void on_wheel( short delta );

	void GetVPos( int x, int y, VPos* vp, bool linemode=false ) const;
	void GetOrigin( int* x, int* y ) const;
	void ConvDPosToVPos( DPos dp, VPos* vp, const VPos* base=NULL ) const;
	void ScrollTo( const VPos& vp );
	int  GetLastWidth( ulong tl ) const;
	int  getNumScrollLines( void );

public:

	const RECT& zone() const { return cvs_.zone(); }
	int left()  const { return cvs_.zone().left; }
	int right() const { return cvs_.zone().right; }
	int bottom()const { return cvs_.zone().bottom; }
	int lna()   const { return cvs_.zone().left; }
	int cx()    const { return cvs_.zone().right - cvs_.zone().left; }
	int cxAll() const { return cvs_.zone().right; }
	int cy()    const { return cvs_.zone().bottom; }

private:

	const DocImpl&   doc_;
	Canvas           cvs_;
	Cursor           cur_;
	gapbufobj<WLine> wrap_;
	ulong            vlNum_;
	ulong            textCx_;
	short            accdelta_;

private:

	void DrawLNA( const VDrawInfo& v, Painter& p );
	void DrawTXT( const VDrawInfo v, Painter& p );
	void Inv( int y, int xb, int xe, Painter& p );

	void CalcEveryLineWidth();
	ulong CalcLineWidth( const unicode* txt, ulong len ) const;
	void ModifyWrapInfo( const unicode* txt, ulong len, WLine& wl, ulong stt );
	void ReWrapAll();
	int ReWrapSingle( const DPos& s );
	int InsertMulti( ulong ti_s, ulong ti_e );
	int DeleteMulti( ulong ti_s, ulong ti_e );
	void UpdateTextCx();
	void ReDraw( ReDrawType r, const DPos* s=NULL );

private:

	HWND hwnd_;
	SCROLLINFO rlScr_; // 横スクロール情報（pixel単位）
	SCROLLINFO udScr_; // 縦スクロール情報（行単位）
	ulong udScr_tl_;   // 一番上に表示される論理行のTLine_Index
	ulong udScr_vrl_;  // 一番上に表示される表示行のVRLine_Index

private:

	bool ReSetScrollInfo();
	void ForceScrollTo( ulong tl );
	void UpdateScrollBar();
	ReDrawType TextUpdate_ScrollBar( const DPos& s, const DPos& e, const DPos& e2 );

	ulong tl2vl( ulong tl ) const;
	void GetDrawPosInfo( VDrawInfo& v ) const;
	void InvalidateView( const DPos& dp, bool afterall ) const;
	void ScrollView( int dx, int dy, bool update );
	void UpDown( int dy, bool thumb );
};



//-------------------------------------------------------------------------

inline void ViewImpl::on_view_resize( int cx, int cy )
	{ DoResize( cvs_.on_view_resize( cx, cy ) ); }

inline void ViewImpl::SetWrapType( int wt )
	{ cvs_.on_config_change( wt, cvs_.showLN() );
	  DoConfigChange(); }

inline void ViewImpl::ShowLineNo( bool show )
	{ cvs_.on_config_change( cvs_.wrapType(), show );
	  DoConfigChange(); }

inline void ViewImpl::SetFont( const VConfig& vc )
	{ cvs_.on_font_change( vc );
	  cur_.on_setfocus();
	  CalcEveryLineWidth(); // 行幅再計算
	  DoConfigChange(); }

inline void ViewImpl::GetOrigin( int* x, int* y ) const
	{ *x = left()-rlScr_.nPos, *y = -udScr_.nPos*cvs_.getPainter().H(); }



//=========================================================================

}}     // namespace editwing::view
#endif // _EDITWING_IP_VIEW_H_
