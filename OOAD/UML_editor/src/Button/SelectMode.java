package Button;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.event.MouseEvent;

import GUI.UML_Canvas;
import Graph.Point;
import Graph.Shape;

public class SelectMode extends Mode{

	//panel background color
	final Color PANEL_BACKGROUND = Color.BLACK;
	final Color SELECT_FOREGROUND = new Color(51, 153, 255); //deep blue
	final Color SELECT_BACKGROUND = new Color(168, 202, 236, 128); //blue
	
	//dragged start point
	boolean start_dragging;
	Point startS, endS;
	
	public SelectMode(UML_Canvas canvas){
		super(canvas);
		start_dragging = false;
	}
	
	@Override
	public void mouseDragged(MouseEvent e) {
		// TODO Auto-generated method stub
    	//Check depth
		int x = e.getX();
		int y = e.getY();
		int d = checkDepth(x, y);
		if(startS == null){
    		Shape top = getTopObject(x, y);
    		if(top != null && top.isSelected()){
    			
				//maintain depth number	
    			if(d == 0){
					shrinkDepth();
			    	d = checkDepth(x, y);
				}
    			top.enSelected();
    			top.setDepth(d);
    			if(start_dragging == false){
    				for(Shape g : _canvas.getList()){
    					if(g.isSelected()){
    						g.enSelected();
    						g.startDragging(x, y);
    						g.setDepth(d - 1);
    					}
    				}
    				start_dragging = true;
    			}else{
    				for(Shape g : _canvas.getList()){
    					if(g.isSelected()){
    						g.enSelected();
    						g.doDragging(x, y);
    						g.setDepth(d - 1);
    					}
    				}
    			}
    		}
    		endS = null;
		}else{
    		if(endS == null) endS = new Point(x, y);
    		else{ endS.x = x; endS.y = y;}
		}
		_canvas.repaint();
	}

	@Override
	public void mouseMoved(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseClicked(MouseEvent e) {
		// TODO Auto-generated method stub
	}

	@Override
	public void mouseEntered(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseExited(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mousePressed(MouseEvent e) {
		// TODO Auto-generated method stub
		int x = e.getX();
		int y = e.getY();
		//cancel start point
    	startS = null;
    	//Check depth
		Shape top = getTopObject(x, y);
		if(top != null){
			if(top.isSelected()){
				//top.unSelected();
			}else{
    			unSelectAll();
				top.enSelected();
			}
		}else{
    		//select multiple object and record start point
    		startS = new Point(x, y);
			unSelectAll();
		}
		_canvas.repaint();
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		// TODO Auto-generated method stub
		int x = e.getX();
		int y = e.getY();
		
		start_dragging = false;
		
		if(startS != null && startS.x != x && startS.y != y){
			endS = new Point(x, y);
			//check all object in this area from start point to end point
			
			//if object is inside the selected area, that means its top left and bottom right
			//points are all inside the selected area
    		Point min = new Point(Integer.MAX_VALUE, Integer.MAX_VALUE);
    		Point max = new Point(Integer.MIN_VALUE, Integer.MIN_VALUE);

    		getMinPoint(min, startS);
    		getMinPoint(min, endS);
    		getMaxPoint(max, startS);
    		getMaxPoint(max, endS);
    		
			for(Shape g: _canvas.getList()){
				if(g.isOutside(min.x, min.y, max.x, max.y)){
					g.enSelected();
				}else{
					g.unSelected();
				}
			}
			
		}//else do nothing
		
    	//clean end Select and End Point
    	startS = null;
    	endS = null;
		_canvas.repaint();
	}

	private void getMinPoint(Point min, Point p){
		if(min.x > p.x) min.x = p.x;
		if(min.y > p.y) min.y = p.y;
	}

	private void getMaxPoint(Point max, Point p){
		if(max.x < p.x) max.x = p.x;
		if(max.y < p.y) max.y = p.y;
	}
	
	@Override
	public void drawGraph(Graphics g) {
		// TODO Auto-generated method stub
    	//draw select rectangle if startS != null and endS != null, which means in select mode
		
    	if(startS != null && endS != null){
    		Point min = new Point(Integer.MAX_VALUE, Integer.MAX_VALUE);
    		
    		int w = Math.abs(startS.x - endS.x);
    		int h = Math.abs(startS.y - endS.y);

    		getMinPoint(min, startS);
    		getMinPoint(min, endS);
    		
    		g.setColor(SELECT_BACKGROUND);
    		g.fillRect(min.x, min.y, w, h);
    		g.setColor(SELECT_FOREGROUND);
    		g.drawRect(min.x, min.y, w, h);
    	}
	}
}
