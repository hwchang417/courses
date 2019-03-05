package Menu;

import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.util.ArrayList;

import GUI.UML_Canvas;
import Graph.Composite;
import Graph.Shape;

public class UnGroupMenu extends Menu{
	
	private static final long serialVersionUID = 1L;

	public UnGroupMenu(UML_Canvas canvas){
		super("UnGroup", canvas);
        this.setMnemonic(KeyEvent.VK_U);
	}
    
	public void actionPerformed(ActionEvent arg0) {
		// TODO Auto-generated method stub

		//check if there is any composite object inside the selected list
		int index = 0;
		ArrayList<Shape> list = _canvas.getList();
		Composite remove = null;
		for(Shape element : list){
			if(element.isSelected()){
				if(element.isGroupped()){
					remove = (Composite)element;
					break;
				}
			}
			index++;
		}
		if(remove != null){
			list.remove(index);
			//push them back to list
			ArrayList<Shape> tmp = remove.getList();
			for(Shape element : tmp){
				list.add(element);
			}
			tmp = null;
		}
		_canvas.repaint();
	}

}
