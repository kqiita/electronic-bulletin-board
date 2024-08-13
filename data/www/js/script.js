/*
*read https://developer.mozilla.org/ja/docs/Web/API/WebSockets_API
 */



let socket = new WebSocket("ws://" + window.location.hostname + ":80/ws");

socket.onopen = (event) => {
    console.log("WebSocket connected");
};
socket.onclose = (event) => {
    console.log("WebSocket disconnected");
};
socket.onmessage = (event) => {
    console.log(`Received: ${event.data}`);
};
socket.onerror = (event) => {
    console.log("WebSocket error: ", event);
};

let textbtn = document.querySelector("#text_submit_btn");
textbtn.onclick = (event) => {
    event.preventDefault();
    let msg = {
        type: "text",
        detail: document.getElementById("input_text").value
    };
    socket.send(JSON.stringify(msg));
};

let filebtn = document.querySelector("#file_submit_btn");
filebtn.onclick = (event) => {
    event.preventDefault();
    let msg = {
        type: "file",
        detail: document.getElementById("input_file").value
    };
    socket.send(JSON.stringify(msg));
};
