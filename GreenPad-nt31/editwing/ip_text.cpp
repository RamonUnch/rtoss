#include "stdafx.h"
#include "ip_doc.h"
using namespace editwing;
using namespace editwing::doc;



//=========================================================================
//---- ip_text.cpp   文字列操作・他
//
//		文字列を挿入したり削除したり…という辺りの処理が
//		このファイルにまとめてある。外向けのインターフェイスの
//		実装もついでにここで。
//
//---- ip_parse.cpp  キーワード解析
//---- ip_wrap.cpp   折り返し
//---- ip_scroll.cpp スクロール
//---- ip_draw.cpp   描画・他
//---- ip_cursor.cpp カーソルコントロール
//=========================================================================



//-------------------------------------------------------------------------
// 公開インターフェイス
//-------------------------------------------------------------------------

Document::Document() : busy_(false)
	{ impl_ = new DocImpl( *this ); }

Document::~Document()
	{}

void Document::Execute( const Command& c )
	{ impl_->Execute( c ); }

void Document::SetKeyword( const unicode* b, ulong s )
	{ impl_->SetKeyword( b, s ); }

void Document::AddHandler( DocEvHandler* h )
	{ impl_->AddHandler( h ); }

void Document::DelHandler( DocEvHandler* h )
	{ impl_->DelHandler( h ); }

void Document::OpenFile( aptr<TextFileR> t )
	{ impl_->OpenFile( t ); }

void Document::SaveFile( TextFileW& t )
	{ impl_->SaveFile( t ); }

void Document::ClearAll()
	{ impl_->ClearAll(); }

ulong Document::tln() const
	{ return impl_->tln(); }

const unicode* Document::tl( ulong i ) const
	{ return impl_->tl( i ); }

ulong Document::len( ulong i ) const
	{ return impl_->len( i ); }

ulong Document::getRangeLength( const DPos& s, const DPos& e ) const
	{ return impl_->getRangeLength( s, e ); }

void Document::getText( unicode* b, const DPos& s, const DPos& e ) const
	{ impl_->getText( b, s, e ); }

bool Document::isUndoAble() const
	{ return impl_->isUndoAble(); }

bool Document::isRedoAble() const
	{ return impl_->isRedoAble(); }

bool Document::isModified() const
	{ return impl_->isModified(); }

void Document::ClearModifyFlag()
	{ impl_->ClearModifyFlag(); }

void Document::Undo()
	{ impl_->Undo(); }

void Document::Redo()
	{ impl_->Redo(); }

void Document::SetUndoLimit( long lim )
	{ impl_->SetUndoLimit( lim ); }



//-------------------------------------------------------------------------
// イベントハンドラ処理
//-------------------------------------------------------------------------

void DocImpl::AddHandler( DocEvHandler* eh ) 
{
	// ハンドラ追加
	pEvHan_.Add( eh );
}

void DocImpl::DelHandler( DocEvHandler* eh )
{
	// 後ろから見て行って…
	const int last = pEvHan_.size() - 1;

	// …見つけたら削除
	for( int i=last; i>=0; --i )
		if( pEvHan_[i] == eh )
		{
			pEvHan_[i] = pEvHan_[last];
			pEvHan_.ForceSize( last );
			break;
		}
}

void DocImpl::Fire_TEXTUPDATE
	( const DPos& s, const DPos& e, const DPos& e2, bool reparsed, bool nmlcmd )
{
	AutoLock lk(this);

	// 全部にイベント通知
	for( ulong i=0, ie=pEvHan_.size(); i<ie; ++i )
		pEvHan_[i]->on_text_update( s, e, e2, reparsed, nmlcmd );
}

void DocImpl::Fire_KEYWORDCHANGE()
{
	AutoLock lk(this);

	// 全部にイベント通知
	for( ulong i=0, ie=pEvHan_.size(); i<ie; ++i )
		pEvHan_[i]->on_keyword_change();
}

void DocImpl::Fire_MODIFYFLAGCHANGE()
{
	AutoLock lk(this);

	// 全部にイベント通知
	bool b = urdo_.isModified();
	for( ulong i=0, ie=pEvHan_.size(); i<ie; ++i )
		pEvHan_[i]->on_dirtyflag_change( b );
}



//-------------------------------------------------------------------------
// UnDo,ReDo 処理
//-------------------------------------------------------------------------

UnReDoChain::Node::Node( Command* c, Node* p, Node* n )
	: cmd_ ( c )
	, prev_( p )
	, next_( n )
{
}

UnReDoChain::Node::Node()
{
	next_ = prev_ = this;
	cmd_  = NULL;
}

UnReDoChain::Node::~Node()
{
	delete cmd_;
}

void UnReDoChain::Node::ResetCommand( Command* cmd )
{
	delete cmd_;
	cmd_ = cmd;
}

UnReDoChain::UnReDoChain()
	: lastOp_  ( &headTail_ )
	, savedPos_( &headTail_ )
	, num_     ( 0 )
	, limit_   ( static_cast<ulong>(-1) )
{
}

UnReDoChain::~UnReDoChain()
{
	Clear();
}

ulong UnReDoChain::Node::ChainDelete(Node*& savedPos_ref)
{
	if( cmd_ == NULL )
		return 0;
	if( savedPos_ref == this )
		savedPos_ref = NULL;
	dptr<Node> d(this);
	return 1 + next_->ChainDelete(savedPos_ref);
}

void UnReDoChain::Clear()
{
	headTail_.next_->ChainDelete(savedPos_);
	headTail_.next_ = headTail_.prev_ = lastOp_  = savedPos_ = &headTail_;
	num_ = 0;
}

void UnReDoChain::SetLimit( long lim )
{
	limit_ = Max( 1UL, ulong(lim) );
}

inline void UnReDoChain::Undo( Document& doc )
{
	lastOp_->ResetCommand( (*lastOp_->cmd_)(doc) );
	lastOp_ = lastOp_->prev_;
}

inline void UnReDoChain::Redo( Document& doc )
{
	lastOp_ = lastOp_->next_;
	lastOp_->ResetCommand( (*lastOp_->cmd_)(doc) );
}

void UnReDoChain::NewlyExec( const Command& cmd, Document& doc )
{
	Command* nCmd = cmd(doc);
	if( nCmd != NULL )
	{
		num_   -= (lastOp_->next_->ChainDelete(savedPos_) - 1);
		lastOp_ = lastOp_->next_ = new Node(nCmd,lastOp_,&headTail_);

		while( limit_ < num_ )
		{
			// 回数制限を越えたので、古い物を削除
			Node* old = headTail_.next_;
			headTail_.next_   = old->next_;
			old->next_->prev_ = &headTail_;
			if( old != &headTail_ )
				delete old;
			if( savedPos_ == &headTail_ )
				savedPos_ = NULL;
			else if( savedPos_ == old )
				savedPos_ = &headTail_;
			--num_;
		}
	}
}



bool DocImpl::isUndoAble() const
	{ return urdo_.isUndoAble(); }

bool DocImpl::isRedoAble() const
	{ return urdo_.isRedoAble(); }

bool DocImpl::isModified() const
	{ return urdo_.isModified(); }

void DocImpl::SetUndoLimit( long lim )
	{ urdo_.SetLimit( lim ); }

void DocImpl::ClearModifyFlag()
{
	bool b = urdo_.isModified();
	urdo_.SavedHere();
	if( b != urdo_.isModified() )
		Fire_MODIFYFLAGCHANGE();
}

void DocImpl::Undo()
{
	if( urdo_.isUndoAble() )
	{
		bool b = urdo_.isModified();
		urdo_.Undo(doc_);
		if( b != urdo_.isModified() )
			Fire_MODIFYFLAGCHANGE();
	}
}

void DocImpl::Redo()
{
	if( urdo_.isRedoAble() )
	{
		bool b = urdo_.isModified();
		urdo_.Redo(doc_);
		if( b != urdo_.isModified() )
			Fire_MODIFYFLAGCHANGE();
	}
}

void DocImpl::Execute( const Command& cmd )
{
	bool b = urdo_.isModified();
	urdo_.NewlyExec( cmd, doc_ );
	if( b != urdo_.isModified() )
		Fire_MODIFYFLAGCHANGE();
}



//-------------------------------------------------------------------------
// カーソル移動ヘルパー
//-------------------------------------------------------------------------

DPos DocImpl::leftOf( const DPos& dp, bool wide ) const
{
	if( dp.ad == 0 )
	{
		// 行の先頭だが、ファイルの先頭ではない場合
		// 一つ前の行の行末へ
		if( dp.tl > 0 )
			return DPos( dp.tl-1, len(dp.tl-1) );
		return dp;
	}
	else if( !wide )
	{
		// 行の途中で、普通に１文字戻る場合
		const unicode* l = tl(dp.tl);
		if( dp.ad>=2 && isLowSurrogate(l[dp.ad-1]) && isHighSurrogate(l[dp.ad-2]) )
			return DPos( dp.tl, dp.ad-2 );
		return DPos( dp.tl, dp.ad-1 );
	}
	else
	{
		// 行の途中で、１単語分戻る場合
		const uchar* f = pl(dp.tl);
			  ulong  s = dp.ad-1;
		while( (f[s]>>5)==0 && 0<=s )
			--s;
		return DPos( dp.tl, s );
	}
}

DPos DocImpl::rightOf( const DPos& dp, bool wide ) const
{
	if( dp.ad == len(dp.tl) )
	{
		// 行末だが、ファイルの終わりではない場合
		// 一つ後の行の先頭へ
		if( dp.tl < tln()-1 )
			return DPos( dp.tl+1, 0 );
		return dp;
	}
	else if( !wide )
	{
		// 行の途中で、普通に１文字進む場合
		const unicode* l = tl(dp.tl);
		// 番兵 0x007f が l の末尾にいるので長さチェックは不要
		if( isHighSurrogate(l[dp.ad]) && isLowSurrogate(l[dp.ad+1]) )
			return DPos( dp.tl, dp.ad+2 );
		return DPos( dp.tl, dp.ad+1 );
	}
	else
	{
		// 行の途中で、普通に１単語進む場合
		const uchar* f = pl(dp.tl);
		const ulong  e = len(dp.tl);
			  ulong  s = dp.ad;
		const ulong  t = (f[s]>>5);
		s += t;
		if( s >= e )
			s = e;
		else if( t==7 || t==0 )
			while( (f[s]>>5)==0 && s<e )
				++s;
		return DPos( dp.tl, s );
	}
}

DPos DocImpl::wordStartOf( const DPos& dp ) const
{
	if( dp.ad == 0 )
	{
		// 行の先頭
		return dp;
	}
	else
	{
		// 行の途中
		const uchar* f = pl(dp.tl);
			  ulong  s = dp.ad;
		while( (f[s]>>5)==0 && 0<=s )
			--s;
		return DPos( dp.tl, s );
	}
}



//-------------------------------------------------------------------------
// 挿入・削除等の作業用関数群
//-------------------------------------------------------------------------

ulong DocImpl::getRangeLength( const DPos& s, const DPos& e )
{
	// とりあえず全部足す
	ulong ans=0, tl=s.tl, te=e.tl;
	for( ; tl<=te; ++tl )
		ans += len(tl);
	// 先頭行の分を引く
	ans -= s.ad;
	// 最終行の分を引く
	ans -= len(te) - e.ad;
	// 改行コード(CRLF)の分を加える
	ans += (e.tl-s.tl) * 2;
	// おしまい
	return ans;
}

void DocImpl::getText( unicode* buf, const DPos& s, const DPos& e )
{
	if( s.tl == e.tl )
	{
		// 一行だけの場合
		text_[s.tl].CopyAt( s.ad, e.ad-s.ad, buf );
		buf[e.ad-s.ad] = L'\0';
	}
	else
	{
		// 先頭行の後ろをコピー
		buf += text_[s.tl].CopyToTail( s.ad, buf );
		*buf++ = L'\r', *buf++ = L'\n';
		// 途中をコピー
		for( ulong i=s.tl+1; i<e.tl; i++ )
		{
			buf += text_[i].CopyToTail( 0, buf );
			*buf++ = L'\r', *buf++ = L'\n';
		}
		// 終了行の先頭をコピー
		buf += text_[e.tl].CopyAt( 0, e.ad, buf );
		*buf = L'\0';
	}
}

void DocImpl::CorrectPos( DPos& pos )
{
	// 正常範囲に収まるように修正
	pos.tl = Min( pos.tl, tln()-1 );
	pos.ad = Min( pos.ad, len(pos.tl) );
}

void DocImpl::CorrectPos( DPos& s, DPos& e )
{
	// 必ずs<=eになるように修正
	if( s > e )
	{
		ulong t;
		t=s.ad, s.ad=e.ad, e.ad=t;
		t=s.tl, s.tl=e.tl, e.tl=t;
	}
}

bool DocImpl::DeletingOperation
	( DPos& s, DPos& e, unicode*& undobuf, ulong& undosiz )
{
	AutoLock lk( this );

	// 位置補正
	CorrectPos( s );
	CorrectPos( e );
	CorrectPos( s, e );

	// 削除される量をカウント
	undosiz = getRangeLength( s, e );

	// Undo操作用バッファ確保
	undobuf = new unicode[undosiz+1];
	getText( undobuf, s, e );

	// 削除る
	if( s.tl == e.tl )
	{
		// 一行内削除
		text_[s.tl].RemoveAt( s.ad, e.ad-s.ad );
	}
	else
	{
		// 先頭行の後ろを削除
		text_[s.tl].RemoveToTail( s.ad );
		// 終了行の残り部分をくっつける
		text_[s.tl].InsertToTail( tl(e.tl)+e.ad, len(e.tl)-e.ad );
		// いらん行を削除
		text_.RemoveAt( s.tl+1, e.tl-s.tl );
	}

	// 再解析
	return ReParse( s.tl, s.tl );
}

bool DocImpl::InsertingOperation
	( DPos& s, const unicode* str, ulong len, DPos& e )
{
	AutoLock lk( this );

	// 位置補正
	CorrectPos( s );

	// よーい、どん！
	e.ad = s.ad;
	e.tl = s.tl;

	// 指定文字列を改行で切り分ける準備
	const unicode* lineStr;
	ulong lineLen;
	UniReader r( str, len, &lineStr, &lineLen );

	// 一行目…
	r.getLine();
	text_[e.tl].InsertAt( e.ad, lineStr, lineLen );
	e.ad += lineLen;

	if( !r.isEmpty() )
	{
		// 二行目〜最終行
		do
		{
			r.getLine();
			text_.InsertAt( ++e.tl, new Line(lineStr,lineLen) );
		} while( !r.isEmpty() );

		// 一行目の最後尾に残ってた文字列を最終行へ
		Line& fl = text_[s.tl];
		Line& ed = text_[e.tl];
		const ulong ln = fl.size()-e.ad;
		if( ln )
		{
			ed.InsertToTail( fl.str()+e.ad, ln );
			fl.RemoveToTail( e.ad );
		}

		// 終了位置記録
		e.ad = lineLen;
	}

	// 再解析
	return ReParse( s.tl, e.tl );
}



//-------------------------------------------------------------------------
// 挿入コマンド
//-------------------------------------------------------------------------

Insert::Insert( const DPos& s, const unicode* str, ulong len, bool del )
	: stt_( s )
	, buf_( str )
	, len_( len )
	, del_( del )
{
}

Insert::~Insert()
{
	if( del_ )
		delete const_cast<unicode*>(buf_);
}

Command* Insert::operator()( Document& d ) const
{
	DocImpl& di = d.impl();

	// 挿入
	DPos s=stt_, e;
	bool aa = di.InsertingOperation( s, buf_, len_, e );

	// イベント発火
	di.Fire_TEXTUPDATE( s, s, e, aa, true );

	// 逆操作オブジェクトを返す
	return new Delete( s, e );
}



//-------------------------------------------------------------------------
// 削除コマンド
//-------------------------------------------------------------------------

Delete::Delete( const DPos& s, const DPos& e )
	: stt_( s )
	, end_( e )
{
}

Command* Delete::operator()( Document& d ) const
{
	DocImpl& di = d.impl();

	// 削除
	unicode* buf;
	ulong    siz;
	DPos s = stt_, e=end_;
	bool aa = di.DeletingOperation( s, e, buf, siz );

	// イベント発火
	di.Fire_TEXTUPDATE( s, e, s, aa, true );

	// 逆操作オブジェクトを返す
	return new Insert( s, buf, siz, true );
}



//-------------------------------------------------------------------------
// 置換コマンド
//-------------------------------------------------------------------------

Replace::Replace(
	const DPos& s, const DPos& e, const unicode* str, ulong len, bool del )
	: stt_( s )
	, end_( e )
	, buf_( str )
	, len_( len )
	, del_( del )
{
}

Replace::~Replace()
{
	if( del_ )
		delete const_cast<unicode*>(buf_);
}

Command* Replace::operator()( Document& d ) const
{
	DocImpl& di = d.impl();

	// 削除
	unicode* buf;
	ulong    siz;
	DPos s=stt_, e=end_;
	bool aa = di.DeletingOperation( s, e, buf, siz );

	// 挿入
	DPos e2;
	aa = (di.InsertingOperation( s, buf_, len_, e2 ) || aa);

	// イベント発火
	di.Fire_TEXTUPDATE( s, e, e2, aa, true );

	// 逆操作オブジェクトを返す
	return new Replace( s, e2, buf, siz, true );
}



//-------------------------------------------------------------------------
// マクロコマンド
//-------------------------------------------------------------------------

Command* MacroCommand::operator()( Document& doc ) const
{
	doc.setBusyFlag();

	MacroCommand* undo = new MacroCommand;
	undo->arr_.ForceSize( size() );
	for( ulong i=0,e=arr_.size(); i<e; ++i )
		undo->arr_[e-i-1] = (*arr_[i])(doc);

	doc.setBusyFlag(false);
	return undo;
}



//-------------------------------------------------------------------------
// ファイルに保存
//-------------------------------------------------------------------------

void DocImpl::SaveFile( TextFileW& tf )
{
	urdo_.SavedHere();
	for( ulong i=0,iLast=tln()-1; i<=iLast; ++i )
		tf.WriteLine( tl(i), len(i), i==iLast );
}



//-------------------------------------------------------------------------
// バッファの内容を全部破棄（暫定）
//-------------------------------------------------------------------------

void DocImpl::ClearAll()
{
	// 全部削除
	Execute( Delete( DPos(0,0), DPos(0xffffffff,0xffffffff) ) );

	// Undo-Redoチェインをクリア
	urdo_.Clear();
	urdo_.SavedHere();
	Fire_MODIFYFLAGCHANGE();
}



//-------------------------------------------------------------------------
// ファイルを開く（暫定）
//-------------------------------------------------------------------------

void DocImpl::OpenFile( aptr<TextFileR> tf )
{
	// ToDo: マルチスレッド化
	//currentOpeningFile_ = tf;
	//thd().Run( *this );

	// 挿入
	DPos e(0,0);

	// unicode buf[1024];
	// Use big buffer (much faster on long lines)
#ifdef WIN64
	static unicode buf[2097152]; // 4MB on x64
#else
	static unicode buf[65536]; // 128KB on i386
#endif

	for( ulong i=0; tf->state(); )
	{
		if( size_t L = tf->ReadLine( buf, countof(buf) ) )
		{
			DPos p(i,0xffffffff);
			InsertingOperation( p, buf, (ulong)L, e );
			i = tln() - 1;
		}
		if( tf->state() == 1 )
		{
			DPos p(i++,0xffffffff);
			InsertingOperation( p, L"\n", 1, e );
		}
	}

	// イベント発火
	Fire_TEXTUPDATE( DPos(0,0), DPos(0,0), e, true, false );
}


void DocImpl::StartThread()
{
/*
	// ToDo:
	aptr<TextFileR> tf = currentOpeningFile_;

	// 挿入
	unicode buf[1024];
	while( !isExitRequested() && tf->state() )
	{
		ulong i, j;
		DPos s(i=tln()-1, j=len(i)), e;
		if( ulong L = tf->ReadLine( buf, countof(buf) ) )
			InsertingOperation( s, buf, L, e );
		if( tf->state() == 1 )
			InsertingOperation( DPos(i,0xffffffff), L"\n", 1, e );

		// イベント発火
		Fire_TEXTUPDATE( s, s, e, true, false );
	}
*/
}

