package Menu;

import java.awt.event.ActionListener;

import javax.swing.JMenuItem;

import GUI.UML_Canvas;


public abstract class Menu extends JMenuItem implements ActionListener{

	private static final long serialVersionUID = 1L;
	protected UML_Canvas _canvas;
	
    public Menu(String name, UML_Canvas canvas){
    	this.setText(name);
    	_canvas = canvas;
    }
}
