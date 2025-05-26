import { useState } from "react";


function ListGroup({my_list, heading}) {  
      const [selectedIndex, setSelectedIndex] = useState(-1);
      const getMesssage = () => {
        return my_list.length == 0 ? <p>No item found</p> : null;
      }
      return (
        <>
          <h1>heading</h1>
          {getMesssage()}
          <ul className="list-group">
            {my_list.map((item, index) => <li className={selectedIndex === index ? "list-group-item active" : "list-group-item"} key={item} onMouseOver={() => { setSelectedIndex(index)}}>{item}</li>)}
          </ul>
        </>
      );
}

export default ListGroup;