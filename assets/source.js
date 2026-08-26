const params = new URLSearchParams(window.location.search);
const sourcePath = params.get("path") || "";
const title = document.querySelector("#source-title");
const pathLabel = document.querySelector("#source-path");
const githubLink = document.querySelector("#github-link");
const codeLines = document.querySelector("#code-lines");
const message = document.querySelector("#source-message");

const allowedPath = /^Examples\/[A-Za-z0-9._/-]+\.(c|h|txt|md)$/;
const githubBase = "https://github.com/Zard-C/CS107/blob/master/";

function setMessage(text) {
  if (message) {
    message.textContent = text;
    message.hidden = false;
  }
}

function hideMessage() {
  if (message) {
    message.hidden = true;
  }
}

function renderSource(path, source) {
  const filename = path.split("/").pop();
  title.textContent = filename;
  pathLabel.textContent = path;
  githubLink.href = githubBase + path;
  githubLink.hidden = false;

  codeLines.replaceChildren();
  for (const line of source.replace(/\n$/, "").split("\n")) {
    const item = document.createElement("li");
    const code = document.createElement("code");
    code.textContent = line || " ";
    item.append(code);
    codeLines.append(item);
  }

  hideMessage();
}

async function loadSource() {
  if (!allowedPath.test(sourcePath) || sourcePath.includes("..")) {
    title.textContent = "Source not found";
    pathLabel.textContent = "Choose an example source file from the homepage.";
    setMessage("The requested source path is not allowed.");
    return;
  }

  try {
    const response = await fetch(sourcePath, { cache: "no-cache" });
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}`);
    }

    const buffer = await response.arrayBuffer();
    const source = new TextDecoder("utf-8").decode(buffer);
    renderSource(sourcePath, source);
  } catch (error) {
    title.textContent = "Unable to load source";
    pathLabel.textContent = sourcePath;
    githubLink.href = githubBase + sourcePath;
    githubLink.hidden = false;
    setMessage("View the file on GitHub if your browser blocks local source loading.");
  }
}

loadSource();
