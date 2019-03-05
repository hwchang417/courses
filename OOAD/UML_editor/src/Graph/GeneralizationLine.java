package Graph;

import java.awt.Graphics;

public class GeneralizationLine extends Line{

	public GeneralizationLine(int d, Port start, Port end) {
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
		drawGenerationLine(g, degree, ex, ey);
		drawPort(g, sx, sy);
		//draw Link from start to end
		g.drawLine(sx, sy, ex, ey);
	}
	
	private static void drawGenerationLine(Graphics g, double degree, int ex, int ey){
		double rad135, rad225;

		int[] xPoints, yPoints;
		int nPoints;
		nPoints = 3;
		xPoints = new int[nPoints];
		yPoints = new int[nPoints];
		
		rad135 = degree + Math.PI * (135.0f) / 180.0f;
		rad225 = degree + Math.PI * (225.0f) / 180.0f;
		
		double vx1 = EDGE_LENGTH * Math.cos(rad225) + ex;
		double vy1 = EDGE_LENGTH * Math.sin(rad225) + ey;
		double vx2 = EDGE_LENGTH * Math.cos(rad135) + ex;
		double vy2 = EDGE_LENGTH * Math.sin(rad135) + ey;
		
		xPoints[0] = ex; 			yPoints[0] = ey; 
		xPoints[1] = (int)vx1;	yPoints[1] = (int)vy1; 
		xPoints[2] = (int)vx2; 	yPoints[2] = (int)vy2; 
		
		g.fillPolygon(xPoints, yPoints, nPoints);
		
		//draw triangle
		//g.drawLine(ex, ey, (int)upper_x, upper_y);
		//g.drawLine(ex, ey, (int)lower_x, (int)lower_y);
		//g.drawLine((int)upper_x, (int)upper_y, (int)lower_x, (int)lower_y);
	}

}
