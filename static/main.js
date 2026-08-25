import { TwistyPlayer } from "https://cdn.cubing.net/js/cubing/twisty";

const container = document.getElementById("cube-container");
document.getElementById("loading-text").remove();

const player = new TwistyPlayer({
    visualization: "3D",
    controlPanel: "none",
    background: "none",
    alg: ""
});
container.appendChild(player);

document.getElementById("solveBtn").onclick = async function() {
    const scramble = document.getElementById("scrambleInput").value;
    const solutionBox = document.getElementById("solution-box");

    if(!scramble) return;
    solutionBox.innerText = "Calculating...";
    player.experimentalSetupAlg = scramble;

    try {
        const response = await fetch('/solve', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ scramble: scramble })
        });
        const data = await response.json();
        
        if(data.error) {
            solutionBox.innerText = "Error: " + data.error;
        } else {
            solutionBox.innerText = data.solution;
            player.alg = data.solution;
            player.play();
        }
    } catch (err) {
        solutionBox.innerText = "Error: " + err.message;
    }
};
