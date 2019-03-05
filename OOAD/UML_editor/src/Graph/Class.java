package Graph;

import java.awt.FontMetrics;
import java.awt.Graphics;

public class Class extends BasicObject{

	public static final int GRAPH_WIDTH = 90, GRAPH_HEIGHT = 90;
	static final String DEFAULT_METHOD = "method name";
	static final String DEFAULT_MEMBER = "member name";

	private Point _drag_diff;
	String _method;
	String _member;
	
	public Class(int d, int x, int y){
		super(d, x, y, GRAPH_WIDTH, GRAPH_HEIGHT);
		_title = DEFAULT_TITLE;
		_method = DEFAULT_METHOD;
		_member = DEFAULT_MEMBER;
		_drag_diff = new Point(0, 0);
	}
	
	public void draw(Graphics g){
		int width_2 = _width >> 1;
		int leftx = _position.x;
		int topy = _position.y;
		
		int first_y = topy + (_height)/3;
		int second_y = topy + (_height<<1)/3;

		//Draw the inside of Rectangle
		g.setColor(_background);
		g.fillRect(leftx, topy, _width, _height);

		//Draw the boundary of Rectangle
		g.setColor(_foreground);
		g.drawRect(leftx, topy, _width, _height);
		
		//Draw two lines
		g.drawLine(leftx, first_y, leftx + _width, first_y);
		g.drawLine(leftx, second_y, leftx + _width, second_y);
		
        //Draw centered text
        FontMetrics fm = g.getFontMetrics();
        double textWidth = fm.getStringBounds(_title, g).getWidth();
        
        g.drawString(_title, (int) (leftx + width_2 - textWidth/2), //shift to (center_x - string_width)
                           (int) (topy + ((first_y - topy)>>1) + fm.getMaxAscent() / 2)); //shift to (center_y + ascent / 2)
        
		//Check selected
        showConnectedPort(g);
	}

	public void setTitle(String title){
		_title = title;
	}

	public void setMethod(String method){
		_method = method;
	}

	public void setMember(String member){
		_member = member;
	}
	
	@Override
	public boolean isInside(int x, int y){
		// TODO Auto-generated method stub
		int tmpx = x - (_position.x);
		int tmpy = y - (_position.y);
		return tmpx >= 0 && tmpx < _width && tmpy >= 0 && tmpy < _height;
	}

	@Override
	public boolean isOutside(int fx, int fy, int tx, int ty) {
		int leftx = _position.x;
		int topy = _position.y;
		int rightx = _position.x + _width;
		int bottomy = _position.y + _height;
		
		return fx <= leftx && fy <= topy && tx >= rightx && ty >= bottomy;
	}

	@Override
	public void startDragging(int x, int y) {
		// TODO Auto-generated method stub
		_drag_diff.x = _position.x - x;
		_drag_diff.y = _position.y - y;
	}

	@Override
	public void doDragging(int x, int y) {
		// TODO Auto-generated method stub
		_position.x = _drag_diff.x + x;
		_position.y = _drag_diff.y + y;
	}

	@Override
	public int getX() {
		// TODO Auto-generated method stub
		return _position.x;
	}

	@Override
	public int getY() {
		// TODO Auto-generated method stub
		return _position.y;
	}

	@Override
	public int getWidth() {
		// TODO Auto-generated method stub
		return _width;
	}

	@Override
	public int getHeight() {
		// TODO Auto-generated method stub
		return _height;
	}
	
}
