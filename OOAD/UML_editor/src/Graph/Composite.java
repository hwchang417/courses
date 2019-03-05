package Graph;

import java.awt.Color;
import java.awt.Graphics;
import java.util.ArrayList;

public class Composite extends Shape{
	
	final int PADDING = 10;
	final int PADDING2 = PADDING << 1;
	
	final Color GROUP_BACKGROUND = new Color(255, 255, 0, 128);
	final Color GROUP_FOREGROUND = new Color(255, 255, 0);

	private ArrayList<Shape> _list;
	
	//boundary
	private Point _drag_diff_min;
	private Point _drag_diff_max;
	private Point _min, _max;
	
	public Composite(int d) {
		super(d, true);
		// TODO Auto-generated constructor stub
		_min = new Point(Integer.MAX_VALUE, Integer.MAX_VALUE);
		_max = new Point(Integer.MIN_VALUE, Integer.MIN_VALUE);
		_list = new ArrayList<Shape>();
		_drag_diff_min = new Point(0, 0);
		_drag_diff_max = new Point(0, 0);
	}

	@Override
	public void draw(Graphics g) {
		// TODO Auto-generated method stub
		if(_list.size() >= 1){
			int width = _max.x - _min.x;
			int height = _max.y - _min.y;
			if(isSelected()){
				for (Shape element : _list) {
					element.enSelected();
					element.draw(g);
				}
			}else{
				for (Shape element : _list) {
					element.unSelected();
					element.draw(g);
				}
			}
			g.setColor(GROUP_BACKGROUND);
			g.fillRect(_min.x - PADDING, _min.y - PADDING, width + PADDING2, height + PADDING2);
			g.setColor(GROUP_FOREGROUND);
			g.drawRect(_min.x - PADDING, _min.y - PADDING, width + PADDING2, height + PADDING2);
		}
	}

	public boolean add(Shape g, int w, int h){
		int x = g.getX();
		int y = g.getY();

		if(x - PADDING <= _min.x) _min.x = x - PADDING;
		if(x + w + PADDING >= _max.x) _max.x = x + w + PADDING; 
		
		if(y - PADDING <= _min.y) _min.y = y - PADDING;
		if(y + h + PADDING >= _max.y) _max.y = y + h + PADDING; 
		
		return _list.add(g);
	}
	
	public void remove(Shape g, int w, int h){
		if(_list.size() == 0) return ;
		
		int x = g.getX();
		int y = g.getY();

		int remove_index = _list.indexOf(g);
		_list.remove(remove_index);
		for (Shape element: _list) {
			x = element.getX();
			y = element.getY();
			
			if(x - PADDING <= _min.x) _min.x = x - PADDING;
			if(x + w + PADDING >= _max.x) _max.x = x + w + PADDING; 
			
			if(y - PADDING <= _min.y) _min.y = y - PADDING;
			if(y + h + PADDING >= _max.y) _max.y = y + h + PADDING; 
		}
		
	}
	
	@Override
	public boolean isInside(int x, int y) {
		// TODO Auto-generated method stub
		boolean result = false;
		for(Shape element: _list){
			if(element.isInside(x, y)){
				result = !result;
				break;
			}
		}
		return result;
	}

	@Override
	public boolean isOutside(int fx, int fy, int tx, int ty) {
		// TODO Auto-generated method stub
		for (Shape element: _list) {
			if(element.isOutside(fx, fy, tx, ty)){
				return true;
			}
		}
		return false;
	}

	@Override
	public void startDragging(int x, int y) {
		// TODO Auto-generated method stub
		_drag_diff_min.x = _min.x - x;
		_drag_diff_min.y = _min.y - y;
		_drag_diff_max.x = _max.x - x;
		_drag_diff_max.y = _max.y - y;
		for(Shape g : _list){
			g.enSelected();
			g.startDragging(x, y);
		}
	}

	@Override
	public void doDragging(int x, int y) {
		// TODO Auto-generated method stub
		_min.x = _drag_diff_min.x + x;
		_min.y = _drag_diff_min.y + y;
		_max.x = _drag_diff_max.x + x;
		_max.y = _drag_diff_max.y + y;
		for(Shape g : _list){
			g.enSelected();
			g.doDragging(x, y);
		}
	}

	@Override
	public int getX() {
		// TODO Auto-generated method stub
		return _min.x;
	}

	@Override
	public int getY() {
		// TODO Auto-generated method stub
		return _min.y;
	}

	@Override
	public int getWidth() {
		// TODO Auto-generated method stub
		return _max.x - _min.x;
	}

	@Override
	public int getHeight() {
		// TODO Auto-generated method stub
		return _max.y - _min.y;
	}

	public int getCount(){
		return _list.size();
	}
	
	public ArrayList<Shape> getList(){
		return _list;
	}

	@Override
	public void setTitle(String title) {
		// TODO Auto-generated method stub
		for(Shape g : _list){
			g.setTitle(title);
		}
	}
	

	public int isInsideConnectPoint(int x, int y){
		return BasicObject.CON_NOT_IN_PORT;
	}
	
}
