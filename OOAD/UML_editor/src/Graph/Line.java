package Graph;

import java.awt.Color;
import java.awt.Graphics;


public abstract class Line extends Shape{
	//line thickness
	final int THICKNESS = 5;
	public static final double EDGE_LENGTH = 10.0f;
	
	//line color
	public static final Color LINK_FOREGROUND = new Color(255, 0, 0);
	
	Port _start;
	Port _end;
	
	public Line(int d, Port start, Port end) {
		super(d, false);
		// TODO Auto-generated constructor stub
		_start = start;
		_end = end;
	}
	
	public static Point getOffset(Port port){
		int x, y;
		switch(port._dir){
		case BasicObject.CON_LEFT: 	x = 0; y = (port._obj.getHeight() >> 1);break;
		case BasicObject.CON_RIGHT: x = port._obj.getWidth(); y = port._obj.getHeight() >> 1;break;
		case BasicObject.CON_TOP: 	x = (port._obj.getWidth() >> 1); y = 0;break;
		case BasicObject.CON_BOTTOM: x = (port._obj.getWidth() >> 1); y = port._obj.getHeight();break;
		default:					x = 0; y = 0; break;
		}
		return new Point(x, y);
	}
	

	public static double getDegree(int ex, int ey, int sx, int sy){
		double degree;
		//tan(theta) = (slope); slope = (y2 - y1) / (x2 - x1);
		degree = Math.atan((double)(ey - sy) / (ex - sx));
		
		//cos() => 1, 2 are positive, 3, 4 are negative
		if(degree < 0.0f){
			if(ey - sy > 0){//4, minus 90.0 degree 
				degree += Math.PI;
			}//2, add 180.0 degree
		}else{
			if(ex - sx < 0){ //3, add 180.0 degree
				degree += Math.PI;
			}//1
		}
		return degree;
	}

	public static void drawPort(Graphics g, int x, int y){
		//draw port
		g.setColor(LINK_FOREGROUND);
		g.fillRect(x - Port.CON_PORT_SIZE_2, y - Port.CON_PORT_SIZE_2, Port.CON_PORT_SIZE, Port.CON_PORT_SIZE);
	}
	
	@Override
	public boolean isInside(int x, int y) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isOutside(int fx, int fy, int tx, int ty) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void startDragging(int x, int y) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void doDragging(int x, int y) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public int getX() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int getY() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int getWidth() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int getHeight() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public void setTitle(String title) {
		// TODO Auto-generated method stub
		
	}
	
	public int isInsideConnectPoint(int x, int y){
		return BasicObject.CON_NOT_IN_PORT;
	}
	
}
