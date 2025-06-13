import { useState } from "react";
import { Panel, PanelGroup, PanelResizeHandle } from "react-resizable-panels";
import styled from "styled-components";


//Button style got from

const theme = {
    blue: {
      default: "#3f51b5",
      hover: "#283593",
    },
    pink: {
      default: "#e91e63",
      hover: "#ad1457",
    },
  };
  
  const Button = styled.button`
    background-color: ${(props) => theme[props.theme].default};
    color: white;
    padding: 5px 15px;
    border-radius: 5px;
    outline: 0;
    border: 0; 
    text-transform: uppercase;
    margin: 10px 0px;
    cursor: pointer;
    box-shadow: 0px 2px 2px lightgray;
    transition: ease background-color 250ms;
    &:hover {
      background-color: ${(props) => theme[props.theme].hover};
    }
    &:disabled {
      cursor: default;
      opacity: 0.7;
    }
  `;
  
  Button.defaultProps = {
    theme: "blue",
  };
  
  function clickMe() {
    alert("You clicked me!");
  }
  
  const ButtonToggle = styled(Button)`
    opacity: 0.7;
    ${({ active }) =>
      active &&
      `
      opacity: 1; 
    `}
  `;
  
  const Tab = styled.button`
    padding: 10px 30px;
    cursor: pointer;
    opacity: 0.6;
    background: white;
    border: 0;
    outline: 0;
    border-bottom: 2px solid transparent;
    transition: ease border-bottom 250ms;
    ${({ active }) =>
      active &&
      `
      border-bottom: 2px solid black;
      opacity: 1;
    `}
  `;
  
  function TabGroup() {
    const [active, setActive] = useState(types[0]);
    return (
      <>
        <div>
          {types.map((type) => (
            <Tab
              key={type}
              active={active === type}
              onClick={() => setActive(type)}
            >
              {type}
            </Tab>
          ))}
        </div>
        <p />
        <p> Your payment selection: {active} </p>
      </>
    );
  }
  
  const types = ["Cash", "Credit Card", "Bitcoin"];
  
  function ToggleGroup() {
    const [active, setActive] = useState(types[0]);
    return (
      <div>
        {types.map((type) => (
          <ButtonToggle active={active === type} onClick={() => setActive(type)}>
            {type}
          </ButtonToggle>
        ))}
      </div>
    );
  }
  


function List({AlgoScripList, onSelectedScript}) {
    return (
        <div className="flex-wrap p-4">
            {AlgoScripList.map((AlgoScript) => (<Button key={AlgoScript.id} onClick={() => onSelectedScript(AlgoScript)} className="px-4 py-2 rounded">
                {AlgoScript.name}
            </Button>))}
        </div>
    );
}

function AlgoScript({selectedScript}) {
    return (
    <div>
        <div className="flex-wrap p-4">
            <Button>
                Save
            </Button>
            <Button className="px-4 py-2 rounded">
                Activate/Deactivate
            </Button>
        </div>
        <div>
            {selectedScript ? 
            (
                <pre>{selectedScript.content}</pre>
            ) :
            (
                <div>Select a file to view its content</div>
            )}
        </div>
    </div>
    )
}

function ScriptsList() {

    const AlgoScriptList = [
        { id: 1, name: 'File 1', content: 'This is the content of File 1.' },
        { id: 2, name: 'File 2', content: 'This is the content of File 2.\nLine 2 of file 2.' },
        { id: 3, name: 'File 3', content: 'This is the content of File 3.' },
        { id: 4, name: 'Script1', content: 'Script1 actual content.' }
    ];

    const [selectedScript, onSelectedScript] = useState(null);

    return (
        <div className="w-75 h-screen">
            <PanelGroup autoSaveId="example" direction="horizontal">

                <Panel defaultSize={25}>
                    <div style={{ minHeight: 600 }} className="listlayout w-75">
                        <List AlgoScripList = {AlgoScriptList} onSelectedScript={onSelectedScript}/>
                    </div>
                </Panel>

                <PanelResizeHandle className="w-2 bg-blue-300 hover:bg-blue-500 transition-colors duration-200 cursor-col-resize" />

                <Panel>
                    <PanelGroup direction="vertical">

                        <Panel>
                            <div>
                                <AlgoScript selectedScript={selectedScript}/>
                            </div>
                        </Panel>

                        <PanelResizeHandle className="h-2 bg-blue-300"/>

                        <Panel>
                            <div>
                                <textarea
                                    className="w-screen h-screen"
                                    placeholder="Summary of the script..."
                                />
                            </div>
                        </Panel>

                    </PanelGroup>
                </Panel>

            </PanelGroup>
        </div>
    );
}

export default ScriptsList;