export async function handleScriptRequest() {
    const request = {
        id : "999",
        qty : 35,
        price : 13.2
    }
    const response = await fetch("http://localhost:8000/request",
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