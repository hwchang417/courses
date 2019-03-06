//CCanvas.cpp
//class for windows 2d graphics
//to be included in engines
//blua tigro 
//14-7-9

#include <windows.h> 
#include <string>

//COLORREF 0x00bbggrr

class CCanvas
{
public:
   CCanvas();
   ~CCanvas( );

   HDC hdc;
   int height;
   int width;
   
   int mouse_X;
   int mouse_Y;
   int mouse_Button;
   char key;
 
   void init(HDC h);
   void pixel( int x , int y , int clr );
   int point( int x , int y );
   void line( int x1 , int y1 , int x2 , int y2 
   , int clr , int thick );
   void ellipse( int mx , int my , int dx , int dy 
   , int clrin , int clrout , int thick );
   void polygon( POINT *pnt , int max 
   , int clrin , int clrout , int thick );
   void text( int x , int y 
   , char* txt
   , char* fontname 
   , int fontsize , int clr );
   void rectangle( int left , int top
   , int right , int bottom 
   , int clrin , int clrout , int thick );
   void rectangle_trans(int left, int top
   , int right ,  int bottom
   , int clrin , int clrout ,  int thick);
   void rectangle_reverse( int left , int top
   , int right , int bottom, int w, int h 
   , int clr , int thick );

   void saveBMP( char* file , HBITMAP bmp );
   HBITMAP loadBMP( char* file );
   void drawBMP( int x , int y , HBITMAP bmp );
   HBITMAP grabBMP( int left , int top 
	  , int width , int height );
/*
   char* toFrame( char* file 
	  , int frame , int max );
*/
private:
   HBRUSH brush;
   HPEN pen;
};

CCanvas::CCanvas() 
{	
   HBRUSH brush = (HBRUSH)GetStockObject( GRAY_BRUSH );
   HPEN pen = (HPEN)GetStockObject( GRAY_BRUSH ); 
}

CCanvas::~CCanvas( ) 
{
   DeleteObject( pen ); 
   DeleteObject( brush ); 
}

void CCanvas::init(HDC h){
	hdc = h;
}
/*
ellipse( mx , my , dx , dy 
, clrin , clrout , thick ) :
center mx my
border dx dy
thicknes thick
area color clrin
border color clrout 
thick thinknes border
example :
ellipse( 50 , 50 , 40 , 40 
, RED , YELLOW , 10 );
*/
void CCanvas::ellipse( 
  int mx , int my , int dx , int dy
, int clrin , int clrout , int thick ) 
{
   HPEN newpen = CreatePen( PS_SOLID , thick , clrout ); 
   HBRUSH newbrush = CreateSolidBrush( clrin ); 
   HPEN oldpen = (HPEN) SelectObject( hdc , newpen ); 
   HBRUSH oldbrush = (HBRUSH) SelectObject( hdc , newbrush ); 
	
   Ellipse( hdc , mx - dx / 2 , my - dy / 2 
	, mx + dx / 2 , my + dy / 2 ); 
	
   SelectObject( hdc, oldpen ); 
   SelectObject( hdc, oldbrush ); 
   DeleteObject( newpen ); 
   DeleteObject( newbrush ); 
}

/*
line( x1 , y1 , x2 , y2 , clr , thick )
draw a line from x1 y1 to x2 y2
thicknes thick color clr .
example:
line( 0 , 0 , 50 , 50 , RED , 4 );
*/
void CCanvas::line( int x1 , int y1 
   , int x2, int y2, int clr , int thick ) 
{
   HPEN pen = CreatePen( PS_SOLID , thick , clr ); 
   HPEN oldpen = (HPEN)SelectObject( hdc , pen ); 
   MoveToEx( hdc , x1 , y1 , 0 ); 
   LineTo( hdc , x2 , y2 ); 
   SelectObject( hdc, oldpen); 
   DeleteObject( pen ); 
}

/*
pixel( x , y , clr ) :
set pixel x y with color clr .
example:
pixel( 50 , 50 , RED );
*/
void CCanvas::pixel( int x , int y , int clr ) 
{
   SetPixel( hdc , x , y , clr );
}

/*
polygon( pnt , max 
, clrin , clrout , thick ) :
draw a polygon with max sides .
example:
POINT p[] = { 0 , 0
			, 50 , 0
			, 0 , 50 };
polygon( p , 3 , RED , YELLOW , 1 );
*/
void CCanvas::polygon( 
  POINT *pnt , int max 
, int clrin , int clrout , int thick ) 
{
   HPEN newpen = CreatePen( PS_SOLID , thick , clrout ); 
   HBRUSH newbrush = CreateSolidBrush( clrin ); 
   HPEN oldpen = (HPEN) SelectObject( hdc , newpen ); 
   HBRUSH oldbrush = (HBRUSH) SelectObject( hdc , newbrush ); 

   SetPolyFillMode( hdc , WINDING );
   Polygon( hdc , pnt , max );

   SelectObject( hdc, oldpen ); 
   SelectObject( hdc, oldbrush ); 
   DeleteObject( newpen ); 
   DeleteObject( newbrush ); 
}

/*
point( x , y ) :
get color from pixel .
example:
color = point( 50 , 50 );
*/
int CCanvas::point( int x , int y )
{
   return GetPixel( hdc , x , y );
}

/*
text( x , y , txt 
, fontname , fontsize , clr );
draw a text left-top x y
to be drawn txt
example:
text( 50 , 50 , "Hello world !"
, "Arial" , RED );
*/
void CCanvas::text( int x , int y
, char* txt 
, char* fontname 
, int fontsize  , int clr )
{
   LOGFONT logfont = { }; 
   // init all members to 0 
   strcpy( logfont.lfFaceName 
   , fontname ); // max 31+1 chars 
   logfont.lfHeight = fontsize; 
   HFONT font = 
	 CreateFontIndirect(&logfont); 
   HFONT oldfont = 
	 (HFONT)SelectObject( hdc , font ); 
   SetBkMode( hdc , TRANSPARENT ); 
   SetTextColor( hdc , clr ); 
   TextOut( hdc , x , y , txt 
   , strlen( txt ) ); 
   SelectObject( hdc , oldfont ); 
   DeleteObject( font ); 
}

/*
rectangle( left , top , right , bottom
, clrin , clrout , thick ) :
draw a rectangle from left top
to right bottom
thicknes border thick
area color clrin
border color clrout .
example:
rectangle( 10 , 10 , 50 , 50 
, RED , YELLOW , 4 );
*/
void CCanvas::rectangle( int left , int top
, int right , int bottom 
, int clrin , int clrout , int thick )
{
   HPEN newpen = CreatePen( PS_SOLID , thick , clrout ); 
   HBRUSH newbrush = CreateSolidBrush( clrin ); 
   HPEN oldpen = (HPEN) SelectObject( hdc , newpen ); 
   HBRUSH oldbrush = (HBRUSH) SelectObject( hdc , newbrush ); 
   
   Rectangle( hdc , left , top 
	, right , bottom ); 
	
   SelectObject( hdc, oldpen ); 
   SelectObject( hdc, oldbrush ); 
   DeleteObject( newpen ); 
   DeleteObject( newbrush ); 
}

void CCanvas::rectangle_trans( int left , int top
, int right , int bottom 
, int clrin , int clrout , int thick )
{
   HPEN newpen = CreatePen( PS_SOLID , thick , clrout ); 
   HPEN oldpen = (HPEN) SelectObject( hdc , newpen ); 
   HBRUSH oldbrush = (HBRUSH) SelectObject( hdc , GetStockObject(NULL_BRUSH) ); 
   //SetBkMode(hdc, TRANSPARENT);
   
   Rectangle( hdc , left , top 
	, right , bottom ); 
	
   //SetBkMode(hdc, OPAQUE);
   SelectObject( hdc, oldpen ); 
   SelectObject( hdc, oldbrush ); 
   DeleteObject( newpen ); 
}

void CCanvas::rectangle_reverse( int left , int top
, int right , int bottom, int w, int h 
, int clr , int thick )
{
   HPEN newpen = CreatePen( PS_NULL , thick , clr ); 
   HBRUSH newbrush = CreateSolidBrush( clr ); 
   //specified pen
   //HPEN oldpen = (HPEN) SelectObject( hdc , GetStockObject(WHITE_PEN) ); 
   //HBRUSH oldbrush = (HBRUSH) SelectObject( hdc , GetStockObject(WHITE_BRUSH) );
   //PS_NULL: no border pen 
   //GetStockObject(NULL_BRUSH): no filled brush
   HPEN oldpen = (HPEN) SelectObject( hdc , newpen ); 
   HBRUSH oldbrush = (HBRUSH) SelectObject( hdc , newbrush ); 
   //SetBkMode(hdc, TRANSPARENT);
   
   Rectangle( hdc , right , top
	, w-1 , bottom ); 
   Rectangle( hdc , left , bottom  
	, w-1 , h-1 ); 
	
   //SetBkMode(hdc, OPAQUE);
   SelectObject( hdc, oldpen ); 
   SelectObject( hdc, oldbrush ); 
   DeleteObject( newpen ); 
   DeleteObject( newbrush ); 
}

/*
void CCanvas::saveBMP( char* file , ? bmp )
{}
*/

/*
HBITMAP CCanvas::loadBMP( char* file )
{
   HBITMAP hbmp = LoadBitmap( g_hInst , file );
   if ( !hbmp )
   {
	  MessageBox( hwnd
	  , "Load of resources failed." , "Error" 
	  , MB_OK | MB_ICONEXCLAMATION );
	  return -1;
   }
}
*/

/*
void CCanvas::drawBMP( int x , int y 
, HBITMAP bmp )
{
   HDC hdcMem;
   hdcMem = CreateCompatibleDC( hdc );
   
   SelectObject( hdcMemory , hbmp);
   BitBlt( hdc , x , y , bmp.bmWidth , bmp.bmHeight
   , hdcMem , 0 , 0 , SRCPAINT );

   DeleteDC( hdcMem );
}
*/

/*
HBITMAP CCanvas::grabBMP( int left , int top
, int width , int height )
{}
*/

/*
//toFrame( "abc" , 13 , 4 ) = "abc0013"
string toFrame( char* file 
, int frame , int max )
{
   return "0";
}
*/



