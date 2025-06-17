import { useState } from "react";
import { Panel, PanelGroup, PanelResizeHandle } from "react-resizable-panels";
import styled from "styled-components";

import createScriptSubmit from "../../structures/ScriptSubmit"
import { handleScriptRequest } from "../../backend-api/ScriptRequestHandler";


//Button style got from https://react.school/ui/button
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

  


function List({AlgoScripList, onSelectedScript}) {
    const handleSelectScript = (AlgoScript) => {
        console.log("neww change after clicking")
        console.log(AlgoScript['content'])
        onSelectedScript(AlgoScript);
    };
    return (
        <div className="flex-wrap p-4">
            {AlgoScripList.map((AlgoScript) => (<Button onClick={() => handleSelectScript(AlgoScript)} className="px-4 py-2 rounded">
                {AlgoScript.title}
            </Button>))}
        </div>
    );
}

function AlgoScript({selectedScript}) {
    const [content, setContent] = useState('');
    const [summary, setSummary] = useState('');
    const handleContentChange = (e) => {
        if (selectedScript) {
            selectedScript.content = e.target.value;
        }
        setContent(e.target.value);
      };
    
    const handleSummaryChange = (e) => {
        if (selectedScript) {
            selectedScript.summary = e.target.value;
        }
        setSummary(e.target.value);
    };

    const handleSave = () => {
        handleScriptRequest(selectedScript);
      };

    return (
        <PanelGroup direction="vertical">
            <Panel>
                <div>
                    <div className="flex-wrap p-4">
                        <Button onClick={handleSave}>
                            Save
                        </Button>
                        <Button className="px-4 py-2 rounded on">
                            Activate/Deactivate
                        </Button>
                    </div>
                    <div>
                        {selectedScript ? 
                        (
                            <textarea className="w-screen h-screen" placeholder="Write your script here..." value={selectedScript.content} onChange={handleContentChange}/>
                        ) :
                        (
                            <div>Select a file to view its content</div>
                        )}
                    </div>
                </div>
            </Panel>

            <PanelResizeHandle className="h-2 bg-blue-300"/>

            <Panel>
                <div>
                    {
                        selectedScript ? 
                            (
                                <textarea className="w-screen h-screen" placeholder="Summary of the script..." value={selectedScript.summary} onChange={handleSummaryChange}/>
                            ) : 
                            (
                                <textarea className="w-screen h-screen" placeholder="Summary of the script..."/>
                            )
                    }
                </div>
            </Panel>
        </PanelGroup>
    )
}

function ScriptsList() {

    const [scriptList, setScriptList] = useState([
        createScriptSubmit("Low Price trigger", "if (price is low) SendBuyOrder(order)", "when the price is low, it will send a buy order", "Jean Baptiste "),
        createScriptSubmit("High Price trigger", "if (price is high) SendSellOrder(order)", "when the price is high, it will send a sell order", "Jean Baptiste"),
        createScriptSubmit("Short Position Send Buy", "if (Position(MYUSER) is short) SendBuyOrder(order)", "We do not accept to be short for some type of instruments", "Jean Baptiste"),
      ]);

    const [selectedScript, onSelectedScript] = useState(null);

    return (
        <div className="w-75 h-screen">
            <PanelGroup autoSaveId="example" direction="horizontal">

                <Panel defaultSize={25}>
                    <div style={{ minHeight: 600 }} className="listlayout w-75">
                        <List AlgoScripList = {scriptList} onSelectedScript={onSelectedScript}/>
                    </div>
                </Panel>

                <PanelResizeHandle className="w-2 bg-blue-300 hover:bg-blue-500 transition-colors duration-200 cursor-col-resize" />

                <Panel>
                    <AlgoScript selectedScript={selectedScript}/>   
                </Panel>

            </PanelGroup>
        </div>
    );
}

export default ScriptsList;