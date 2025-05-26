import ListGroup from "./components/ListGroup";

function App() {
  let my_list = [
    'Paris',
    'New York',
    'London',
    'Warsaw'
  ];
  return (
    <div><ListGroup my_list = {my_list} heading='cities' /></div>
  );
}

export default App