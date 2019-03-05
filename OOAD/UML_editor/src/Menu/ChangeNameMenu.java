package Menu;

import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.util.ArrayList;

import javax.swing.JOptionPane;

import GUI.UML_Canvas;
import Graph.BasicObject;
import Graph.Shape;

public class ChangeNameMenu extends Menu{
	
	private static final long serialVersionUID = 1L;
	public static final int MAX_NAME_COUNT = 10;
	
	public ChangeNameMenu(UML_Canvas canvas){
		super("Change Name", canvas);
        this.setMnemonic(KeyEvent.VK_C);
	}
    
	public void actionPerformed(ActionEvent arg0) {
		// TODO Auto-generated method stub

		boolean onlyOneSelected = false;
		Shape selected = null;
		ArrayList<Shape> list = _canvas.getList();
		for(Shape g : list){
			if(g.isSelected()){
				onlyOneSelected = !onlyOneSelected;
				if(!onlyOneSelected)  break;
				selected = g;
			}
		}
		if(onlyOneSelected){
			String input = null;
			while(input == null){
				input = JOptionPane.showInputDialog("½Ð¿é¤JClass¦WºÙ:");
				if(input == null) break;
				if(input.length() > MAX_NAME_COUNT) input = null;
			}

			if(input != null){
				selected.setTitle(input);
				_canvas.repaint();
			}
		}
	}

}
