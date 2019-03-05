package Graph;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;
import java.awt.geom.Path2D;

public class CompositionLine extends Line{

	public CompositionLine(int d, Port start, Port end) {
		super(d, start, end);
		// TODO Auto-generated constructor stub
	}

	@Override
	public void draw(Graphics g) {
		// TODO Auto-generated method stub
		Point start_offset = getOffset(_start);
		Point end_offset = getOffset(_end);
		int sx, sy, ex, ey;
		
		sx = _start._obj.getX() + start_offset.x;
		sy = _start._obj.getY() + start_offset.y;
		ex = _end._obj.getX() + end_offset.x;
		ey = _end._obj.getY() + end_offset.y;
		
		drawLineFromPoint(g, sx, sy, ex, ey);
	}
	
	public static void drawLineFromPoint(Graphics g, int sx, int sy, int ex, int ey){
		double degree = getDegree(ex, ey, sx, sy);
		//draw end symbol by its type
		g.setColor(LINK_FOREGROUND);
		drawCompositionLine(g, degree, ex, ey);
		drawPort(g, sx, sy);
		//draw Link from start to end
		g.drawLine(sx, sy, ex, ey);
	}
	
	private static void drawCompositionLine(Graphics g, double degree, int ex, int ey){
		double rad135, rad225;
		int x, y;
		rad135 = degree + Math.PI * (135.0f) / 180.0f;
		rad225 = degree + Math.PI * (225.0f) / 180.0f;
		
		double vx1 = EDGE_LENGTH * Math.cos(rad225) + ex;
		double vy1 = EDGE_LENGTH * Math.sin(rad225) + ey;
		double vx2 = EDGE_LENGTH * Math.cos(rad135) + ex;
		double vy2 = EDGE_LENGTH * Math.sin(rad135) + ey;
		double vx3 = vx1 + (vx2 - ex); //vector AD == vector BC
		double vy3 = vy1 + (vy2 - ey);
		
        Graphics2D g2d = (Graphics2D) g.create();
        
        Path2D.Double diamond = new Path2D.Double();
        diamond.moveTo(ex, ey);
        diamond.lineTo(vx1, vy1);
        diamond.lineTo(vx3, vy3);
        diamond.lineTo(vx2, vy2);
        diamond.closePath();

        x = ((int)EDGE_LENGTH - diamond.getBounds().width) / 2;
        y = ((int)EDGE_LENGTH - diamond.getBounds().height) / 2;
        
        AffineTransform at = AffineTransform.getTranslateInstance(x, y);
        java.awt.Shape shape = at.createTransformedShape(diamond);
        g2d.fill(shape);
        g2d.draw(shape);
        g2d.dispose();
	}
}
