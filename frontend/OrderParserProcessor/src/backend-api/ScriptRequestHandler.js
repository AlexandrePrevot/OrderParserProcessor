export async function handleScriptRequest(request) {
    console.log(request)
    const response = await fetch("http://localhost:8000/ScriptRequest",
        {   
            method: "POST",
            headers: {"Content-Type" : "application/json"}, // important for JSON data
            body: JSON.stringify(request), // Convert JS object to JSON string
        }
    );

    if (!response.ok) {
        throw new Error("Failed to fetch");
    }
  
    return response.json();
}