package Graph;

import java.awt.Color;
import java.awt.Graphics;

public abstract class Shape {
	private int _depth;
	private boolean _selected;
	private boolean _groupped;
	
	//default color
	protected final Color DEFAULT_BACKGROUND = new Color(0x00, 0xFA, 0x9A);
	protected final Color DEFAULT_FOREGROUND = new Color(0x00, 0x00, 0x00);
	protected final Color DEFAULT_CONNECTED_PORT = new Color(0xFF, 0x00, 0x00);
	protected Color _background;
	protected Color _foreground;
	protected Color _foreground_port;
	
	public abstract void draw(Graphics g);
	public abstract boolean isInside(int x, int y);
	public abstract boolean isOutside(int fx, int fy, int tx, int ty);
	public abstract void startDragging(int x, int y);
	public abstract void doDragging(int x, int y);
	public abstract int getX();
	public abstract int getY();
	public abstract int getWidth();
	public abstract int getHeight();
	public abstract void setTitle(String title);
	public abstract int isInsideConnectPoint(int x, int y);

	public Shape(int d, boolean groupped){ 
		_depth = d; 
		_selected = false; 
		_groupped = groupped;
		_background = DEFAULT_BACKGROUND;
		_foreground = DEFAULT_FOREGROUND;
		_foreground_port = DEFAULT_CONNECTED_PORT;
	}
	
	public void setDepth(int d){
		_depth = d;
	}

	public int getDepth(){
		return _depth;
	}
	
	public void enSelected(){
		_selected = true;
	}
	
	public void unSelected(){
		_selected = false;
	}
	
	public boolean isSelected(){
		return _selected;
	}

	public boolean isGroupped(){
		return _groupped;
	}
	
	public void setBackground(Color color){
		_background = color;
	}
	
	public void setForeground(Color color){
		_foreground = color;
	}
	
	public void setPortColor(Color color){
		_foreground_port = color;
	}
}
