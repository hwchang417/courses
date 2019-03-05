package GUI;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.ArrayList;

import javax.swing.JPanel;

import API.Singleton;
import Button.Mode;
import Graph.Shape;

public class UML_Canvas extends JPanel implements MouseListener, MouseMotionListener{
	private static final long serialVersionUID = 1L;
	private Mode _mode;
	private ArrayList<Shape> _list;
	//panel background color
	final Color PANEL_BACKGROUND = Color.BLACK;
	final int GRID_SIZE = 10;
	
	public UML_Canvas(){
		_mode = Mode.NOSELECTED;
		_list = new ArrayList<Shape>();
    	addMouseListener(this);
    	addMouseMotionListener(this);
	}
	
	public void setMode(Mode mode){
		_mode = mode;
	}
	
	public Mode getMode(){
		return _mode;
	}
	
	public ArrayList<Shape> getList(){
		return _list;
	}
	
	public void setList(ArrayList<Shape> list){
		_list = list;
	}
	
    private void drawGrid(Graphics g){
    	int i, j;
    	int width, height;
    	g.setColor(Color.gray);
    	width = getWidth();
    	height = getHeight();
    	
    	for(i = 0;i <= height;i+=GRID_SIZE){
    		for(j = 0;j <= width;j+=GRID_SIZE){
    			g.drawLine(j, 0, j, height);
    		}
			g.drawLine(0, i, width, i);
    	}
    }
    
    public void paintComponent (Graphics g) {
    	super.paintComponent(g);
    	
    	//black background
    	g.setColor(PANEL_BACKGROUND);
    	g.fillRect(0, 0, getWidth(), getHeight());
    	
    	//draw grid
    	drawGrid(g);
    	
    	//call draw() method by its depth(99 -> 98 -> ... -> 0)
    	Singleton.getInstance().sort(_list);
    	drawList(g);
    	
    	//draw mode's temporary grah
    	if(_mode != Mode.NOSELECTED) _mode.drawGraph(g);
    }

	public void drawList(Graphics g){
		for (Shape element: _list) {
			element.draw(g);
		}
	}

    public void unSelectAll(){
		for (Shape g: _list) {
			g.unSelected();
		}
		repaint();
	}
	
	public void mouseDragged(MouseEvent arg0) {
		// TODO Auto-generated method stub
		if(_mode != Mode.NOSELECTED) _mode.mouseDragged(arg0);
	}

	public void mouseMoved(MouseEvent arg0) {
		// TODO Auto-generated method stub
		if(_mode != Mode.NOSELECTED) _mode.mouseMoved(arg0);
	}

	public void mouseClicked(MouseEvent arg0) {
		// TODO Auto-generated method stub
		if(_mode != Mode.NOSELECTED) _mode.mouseClicked(arg0);
	}

	public void mouseEntered(MouseEvent arg0) {
		// TODO Auto-generated method stub
		if(_mode != Mode.NOSELECTED) _mode.mouseEntered(arg0);
	}

	public void mouseExited(MouseEvent arg0) {
		// TODO Auto-generated method stub
		if(_mode != Mode.NOSELECTED) _mode.mouseExited(arg0);
	}

	public void mousePressed(MouseEvent arg0) {
		// TODO Auto-generated method stub
		if(_mode != Mode.NOSELECTED) _mode.mousePressed(arg0);
	}

	public void mouseReleased(MouseEvent arg0) {
		// TODO Auto-generated method stub
		if(_mode != Mode.NOSELECTED) _mode.mouseReleased(arg0);
	}
	
	
}
