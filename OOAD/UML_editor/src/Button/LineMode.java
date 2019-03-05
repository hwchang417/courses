package Button;

import java.awt.Graphics;
import java.util.ArrayList;

import GUI.UML_Canvas;
import Graph.AssociationLine;
import Graph.BasicObject;
import Graph.CompositionLine;
import Graph.GeneralizationLine;
import Graph.Line;
import Graph.Point;
import Graph.Port;
import Graph.Shape;

public abstract class LineMode extends Mode{

	public static final int TYPE_ASSOCIATION = 1;
	public static final int TYPE_GENERALIZATION = 2;
	public static final int TYPE_COMPOSITION = 3;
	
	Port start_obj, end_obj;
	Point start, end; 
	boolean draw_flag;
	
	public LineMode(UML_Canvas canvas){
		super(canvas);
		draw_flag = false;
	}
	
	public void mouseDragged(int x, int y){ 
		if(draw_flag){
			end.x = x;
			end.y = y;
			_canvas.repaint();
		}
	}

	public void mousePressed(int d, int x, int y) {
		// TODO Auto-generated method stub
		boolean not_inside = true;
    	ArrayList<Shape> list = _canvas.getList();
    	for(Shape g : list){
    		if(g.isInside(x, y)){
    			int dir = g.isInsideConnectPoint(x, y);
    			not_inside = false;
    			if(start_obj == null && dir != BasicObject.CON_NOT_IN_PORT){
        			BasicObject obj = (BasicObject)g;
    				start_obj = new Port(obj, dir);
    				
    				start = Line.getOffset(start_obj);
    				start.x += obj.getX();
    				start.y += obj.getY();
    				end = new Point(x, y);
    				
    				draw_flag = true;
    			}
    			_canvas.repaint();
    			break;
    		}
    	}
    	if(not_inside){
    		start_obj = null;
    		end_obj = null;
			draw_flag = false;
    	}
	}

	public void mouseReleased(int d, int x, int y, int type) {
		// TODO Auto-generated method stub
		boolean not_inside = true;
    	ArrayList<Shape> list = _canvas.getList();
    	for(Shape g : list){
    		if(g.isInside(x, y)){
    			int dir = g.isInsideConnectPoint(x, y);
    			not_inside = false;
    			if(start_obj != null && g != start_obj._obj
    					&& dir != BasicObject.CON_NOT_IN_PORT){
        			BasicObject obj = (BasicObject)g;
    				end_obj = new Port(obj, dir);
    				
    				switch(type){
    					case TYPE_ASSOCIATION: 
    						list.add(new AssociationLine(d, start_obj, end_obj)); 
    						break;
    					case TYPE_GENERALIZATION:
    						list.add(new GeneralizationLine(d, start_obj, end_obj)); 
    						break;
    					case TYPE_COMPOSITION:
    						list.add(new CompositionLine(d, start_obj, end_obj)); 
    						break;
    					default: break;
    				}
    				start_obj = null;
    				end_obj = null;
    				
    				draw_flag = false;
    				_canvas.setList(list);
    			}
    			_canvas.repaint();
    			break;
    		}
    	}
    	if(not_inside){
    		start_obj = null;
    		end_obj = null;
			draw_flag = false;
			_canvas.repaint();
    	}
	}

	public void drawGraph(Graphics g, int type) {
		// TODO Auto-generated method stub
    	if(draw_flag){
			switch(type){
			case TYPE_ASSOCIATION: 
	    		AssociationLine.drawLineFromPoint(g, start.x, start.y, end.x, end.y);
				break;
			case TYPE_GENERALIZATION:
				GeneralizationLine.drawLineFromPoint(g, start.x, start.y, end.x, end.y);
				break;
			case TYPE_COMPOSITION:
				CompositionLine.drawLineFromPoint(g, start.x, start.y, end.x, end.y);
				break;
			default: break;
			}
    	}
	}
}
