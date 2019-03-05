package Graph;

import java.awt.Graphics;

public abstract class BasicObject extends Shape{

	public static final int CON_LEFT = 1;
	public static final int CON_TOP = 2;
	public static final int CON_RIGHT = 3;
	public static final int CON_BOTTOM = 4;
	public static final int CON_NOT_IN_PORT = 0;
	
	static final String DEFAULT_TITLE = "Place an Order";
	protected String _title;
	protected Point _position;
	protected int _width, _height;
	
	public BasicObject(int depth, int x, int y, int w, int h){
		super(depth, false);
		_title = DEFAULT_TITLE;
		_position = new Point(x, y);
		_width = w;
		_height = h;
	}
	
	public void setTitle(String title){
		_title = title;
	}

	public void showConnectedPort(Graphics g){
		if(isSelected()){
			int width_2 = _width >> 1;
			int height_2 = _height >> 1;
			
			g.setColor(_foreground_port);
			g.drawRect(_position.x + width_2 - Port.CON_PORT_SIZE_2, _position.y - Port.CON_PORT_SIZE_2, Port.CON_PORT_SIZE, Port.CON_PORT_SIZE);
			g.drawRect(_position.x + _width - Port.CON_PORT_SIZE_2, _position.y + height_2 - Port.CON_PORT_SIZE_2, Port.CON_PORT_SIZE, Port.CON_PORT_SIZE);
			g.drawRect(_position.x - Port.CON_PORT_SIZE_2, _position.y + height_2 - Port.CON_PORT_SIZE_2, Port.CON_PORT_SIZE, Port.CON_PORT_SIZE);
			g.drawRect(_position.x + width_2 - Port.CON_PORT_SIZE_2, _position.y + _height - Port.CON_PORT_SIZE_2, Port.CON_PORT_SIZE, Port.CON_PORT_SIZE);
		}
	}
	
	public int isInsideConnectPoint(int x, int y){
		int dir;
		int center_x = _position.x + (_width >> 1);
		int center_y = _position.y + (_height >> 1);
		int dx = x - center_x;
		int dy = y - center_y;
		double b1, b2;
		//y = m * x + b;
		//b1 = 130 deg
		b1 = (double)dy + 1.192f * dx;
		//b2 = 40 deg
		b2 = (double)dy - 0.839f * dx;
		
		if(b1 > 0.0f){
			if(b2 > 0.0f)	dir = CON_BOTTOM;
			else 			dir = CON_RIGHT;
		}else{
			if(b2 < 0.0)	dir = CON_TOP;
			else			dir = CON_LEFT;
		}
		
		return dir;
	}
}
