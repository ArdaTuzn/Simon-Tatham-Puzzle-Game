/* ******************** global variables ******************** */
const canvas = document.getElementById('mycanvas');
const ctx = canvas.getContext('2d');
let currentGame = null;
let cellSize = 0;

// Images of pieces
const pieceImages = {
    0: new Image(), // EMPTY
    1: new Image(), // ENDPOINT
    2: new Image(), // SEGMENT
    3: new Image(), // CORNER
    4: new Image(), // TEE
    5: new Image()  // CROSS
};

// Load images
pieceImages[0].src = "src/images/empty.png";
pieceImages[1].src = "src/images/endpoint.png";
pieceImages[2].src = "src/images/segment.png";
pieceImages[3].src = "src/images/corner.png";
pieceImages[4].src = "src/images/tee.png";
pieceImages[5].src = "src/images/cross.png";

/* ******************** register events ******************** */
window.addEventListener('load', windowLoad);
canvas.addEventListener('click', handleCanvasClick);

/* ******************** event callback ******************** */
function windowLoad() {
    console.log("window loaded");
    Module.onRuntimeInitialized = () => {
        currentGame = Module._new_default();
        resizeCanvas(); 
        drawGame(currentGame);
        updateGameStatus();
        if (typeof printGame === 'function') printGame(currentGame);
    };
}

// Handle canvas resizing
function resizeCanvas() {
    if (!currentGame) return;

    const nbRows = Module._nb_rows(currentGame);
    const nbCols = Module._nb_cols(currentGame);

    // Use almost the entire window size
    const maxCanvasWidth = window.innerWidth * 0.9; // 90% width
    const maxCanvasHeight = window.innerHeight * 0.6; // 60% height

    const cellSizeWidth = maxCanvasWidth / nbCols;
    const cellSizeHeight = maxCanvasHeight / nbRows;

    cellSize = Math.min(cellSizeWidth, cellSizeHeight); // Always use squares

    canvas.width = cellSize * nbCols;
    canvas.height = cellSize * nbRows;

    drawGame(currentGame); 
}

function handleCanvasClick(e) {
    if (!currentGame) return;

    const rect = canvas.getBoundingClientRect();
    const x = Math.floor((e.clientX - rect.left) / cellSize);
    const y = Math.floor((e.clientY - rect.top) / cellSize);

    Module._play_move(currentGame, y, x, 1);
    drawGame(currentGame);
    updateGameStatus();
    if (typeof printGame === 'function') printGame(currentGame); 
}

/* ******************** canvas drawing ******************** */
function drawGame(g) {
    const nbRows = Module._nb_rows(g);
    const nbCols = Module._nb_cols(g);

    cellSize = Math.min(canvas.width / nbCols, canvas.height / nbRows);
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    drawGrid(nbRows, nbCols);

    for (let row = 0; row < nbRows; row++) {
        for (let col = 0; col < nbCols; col++) {
            drawPiece(g, row, col);
        }
    }
}

function drawGrid(rows, cols) {
    ctx.save();
    ctx.strokeStyle = '#333';
    ctx.lineWidth = 2;

    for (let i = 0; i <= rows; i++) {
        ctx.beginPath();
        ctx.moveTo(0, i * cellSize);
        ctx.lineTo(cols * cellSize, i * cellSize);
        ctx.stroke();
    }

    for (let j = 0; j <= cols; j++) {
        ctx.beginPath();
        ctx.moveTo(j * cellSize, 0);
        ctx.lineTo(j * cellSize, rows * cellSize);
        ctx.stroke();
    }

    ctx.restore();
}

function drawPiece(g, row, col) {
    const shape = Module._get_piece_shape(g, row, col);
    const orientation = Module._get_piece_orientation(g, row, col);
    const img = pieceImages[shape];

    if (!img.complete) return;

    const x = col * cellSize;
    const y = row * cellSize;
    const padding = cellSize * 0.1;
    const size = cellSize - 2 * padding;

    ctx.save();
    ctx.translate(x + cellSize / 2, y + cellSize / 2);
    ctx.rotate(orientation * Math.PI / 2);
    ctx.drawImage(img, -size / 2, -size / 2, size, size);
    ctx.restore();
}

/* ******************** game controls ******************** */
function restartGame() {
    if (!currentGame) return;
    Module._restart(currentGame);
    drawGame(currentGame);
    updateGameStatus();
    if (typeof printGame === 'function') printGame(currentGame); 
}

function undoMove() {
    if (!currentGame) return;
    Module._undo(currentGame);
    drawGame(currentGame);
    updateGameStatus();
    if (typeof printGame === 'function') printGame(currentGame);
}

function redoMove() {
    if (!currentGame) return;
    Module._redo(currentGame);
    drawGame(currentGame);
    updateGameStatus();
    if (typeof printGame === 'function') printGame(currentGame);
}

function solveGame() {
    if (!currentGame) return;
    Module._solve(currentGame);
    drawGame(currentGame);
    updateGameStatus();
    if (typeof printGame === 'function') printGame(currentGame); 
}

function randomGame() {
    if (!currentGame) return;

    const rows = parseInt(document.getElementById("rows-input").value);
    const cols = parseInt(document.getElementById("cols-input").value);

    if (isNaN(rows) || isNaN(cols) || rows < 2 || cols < 2) {
        return;
    }

    const newGame = Module._new_random(rows, cols, false, 2, 1);
    Module._delete(currentGame);
    currentGame = newGame;
    resizeCanvas(); 
    drawGame(currentGame);

    if (typeof printGame === 'function') printGame(currentGame); 
}

/* ******************** status update ******************** */
function updateGameStatus() {
    const statusDiv = document.getElementById("game-status");
    if (Module._won(currentGame)) {
        statusDiv.style.display = "block";   
        statusDiv.textContent = "Game Won!"; 
    } else {
        statusDiv.style.display = "none";    
    }
}

/* ******************** bind buttons ******************** */
document.getElementById('restart').addEventListener('click', restartGame);
document.getElementById('undo').addEventListener('click', undoMove);
document.getElementById('redo').addEventListener('click', redoMove);
document.getElementById('solve').addEventListener('click', solveGame);
document.getElementById('random').addEventListener('click', randomGame);
