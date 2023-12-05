//https://landgreen.github.io/physics/lib/randomColor.min.js
"use strict";
//setup
let canvas = document.getElementById('canvas-1');
let ctx = canvas.getContext("2d");
//greeting
ctx.textAlign = "center";
ctx.font = '40px sans-serif';
ctx.fillStyle = '#555';
ctx.fillText("click for sneks", canvas.width / 2, canvas.height / 2);

//___________________get keyboard input___________________
var keys = [];
document.onkeydown = function(e) {
  keys[e.keyCode] = true;
  snek.input();
  wurm.input();
  //console.log(e.keyCode);
}
document.onkeyup = function(e) {
  keys[e.keyCode] = false;
  snek.input();
  wurm.input();
}

const game = {
  delay: 4,
  run: true,
  count: true,
  size: 10,
  runSnek: function(el) {
    el.onclick = null; //removes the onclick effect from canvas so the function only runs once
    snek.setup();
    wurm.setup();
    goal.move();
    this.run = true;
    loop();
  },

}

function loop() {
  if (game.run) {
    game.count++
      if (!(game.count % game.delay)) {
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        snek.hitGoal();
        wurm.hitGoal();
        snek.move();
        wurm.move();
        snek.draw();
        wurm.draw();
        goal.draw();
        snek.collision();
        wurm.collision();
        
      }
    requestAnimationFrame(loop);
  }
}

const goal = {
  pos: {
    x: 0,
    y: 10 * 10
  },
  move: function() {
    let notClear = true
    while (notClear) {
      notClear = false;
      this.pos = {
        x: Math.floor(Math.random() * canvas.width / game.size) * game.size,
        y: Math.floor(Math.random() * canvas.height / game.size) * game.size
      };
    }
  },
  draw: function() {
    ctx.fillStyle = '#0aa';
    ctx.fillRect(this.pos.x, this.pos.y, game.size, game.size);
    //ctx.fillRect(this.pos.x-game.size*0.15, this.pos.y-game.size*0.15, game.size*1.3, game.size*1.3);
  }
}

const snek = { //blue
  score: 0,
  size: game.size,
  pos: [],
  draw: function() {
    ctx.beginPath();
    for (let i = 0, len = this.pos.length; i < len; ++i) {
      ctx.rect(this.pos[i].x, this.pos[i].y, this.size, this.size);
    }
    ctx.fillStyle = '#aaa';
    ctx.fill();
    ctx.strokeStyle = '#000';
    ctx.stroke();

    //     for (let i = this.pos.length - 1, c = 99994; i > 0; --i) {
    //       c--
    //       ctx.beginPath();
    //       ctx.rect(this.pos[i].x, this.pos[i].y, this.size, this.size);
    //       if (c % 6 > 2) {
    //         ctx.fillStyle = '#66f';
    //       } else {
    //         ctx.fillStyle = '#000';
    //       }
    //       ctx.fill();
    //    }

  },
  velocity: {
    x: 0,
    y: 0
  },
  vDir: {
    x: 0,
    y: 0
  },
  input: function() {
    if ((keys[37]) && this.velocity.x != this.size) { //left
      this.vDir = {
        x: -this.size,
        y: 0
      }
    } else if ((keys[38]) && this.velocity.y != this.size) { //up
      this.vDir = {
        x: 0,
        y: -this.size
      }
    } else if ((keys[39]) && this.velocity.x != -this.size) { //right
      this.vDir = {
        x: this.size,
        y: 0
      }
    } else if ((keys[40]) && this.velocity.y != -this.size) { //down
      this.vDir = {
        x: 0,
        y: this.size
      }
    }
  },
  move: function() {
    if (this.velocity) {
      this.velocity.x = this.vDir.x;
      this.velocity.y = this.vDir.y;
      this.pos.shift(); //removes a block from the front
      this.pos.push({ //adds a new block to the end
        x: this.pos[this.pos.length - 1].x + this.velocity.x,
        y: this.pos[this.pos.length - 1].y + this.velocity.y
      })
    }
  },
  addBlocks: function(num) {
    for (let i = 0; i < num; ++i) {
      this.pos.unshift({
        x: this.pos[0].x,
        y: this.pos[0].y
      })
    }
  },
  hitGoal: function() {
    if (this.pos[this.pos.length - 1].x === goal.pos.x && this.pos[this.pos.length - 1].y === goal.pos.y) {
      goal.move()
      this.addBlocks(Math.ceil(this.pos.length * 0.1) + 10);
      this.score++;
      document.getElementById("blue").innerHTML = this.score;
    }
  },
  collision: function() {
    const len = this.pos.length - 1;
    //hitting walls
    // if (this.pos[len].x < 0 || this.pos[len].x > canvas.width - this.size || this.pos[len].y < 0 || this.pos[len].y > canvas.height - game.size) {
    //   this.gameOver();
    // }
    //around the world
    if (this.pos[len].x < 0) this.pos[len].x = canvas.width - this.size;
    if (this.pos[len].x > canvas.width - this.size) this.pos[len].x = 0;
    if (this.pos[len].y < 0) this.pos[len].y = canvas.height - this.size;
    if (this.pos[len].y > canvas.height - this.size) this.pos[len].y = 0;
    for (let i = 0; i < len; ++i) { //head of snek hitting snek body
      if ((this.pos[len].x === this.pos[i].x && this.pos[len].y === this.pos[i].y)) {
        this.gameOver();
      }
    }
    for (let i = 0; i < wurm.pos.length; ++i) { //head of snek hitting wurm body
      if ((this.pos[len].x === wurm.pos[i].x && this.pos[len].y === wurm.pos[i].y)) {
        this.gameOver();
      }
    }
  },
  respawn: function() {
    this.score--;
    //document.getElementById("blue").innerHTML = this.score;
    // this.vDir = {
    //   x: 0,
    //   y: this.size
    // }
    // velocity = {
    //     x: 0,
    //     y: this.size
    //   }
    this.setup();
   // this.pos.length = 0;
    // this.pos.push({ //adds a new block to the end
    //   x: Math.round(canvas.width - this.size * 10),
    //   y: this.size
    // })
    // this.pos.push({ //adds a new block to the end
    //   x: Math.round(canvas.width - this.size * 10),
    //   y: this.size
    // })
      //this.pos.splice(13, this.pos.length-13); 

  },
  gameOver: function() {
    this.score--;
    document.getElementById("blue").innerHTML = this.score;
    game.run = false;
    ctx.fillStyle = '#f0f';
    ctx.fillRect(this.pos[this.pos.length - 1].x, this.pos[this.pos.length - 1].y, this.size, this.size);
    ctx.fillStyle = '#555';
    ctx.font = '20px sans-serif';
    ctx.fillText("click for sneks", canvas.width / 2, canvas.height - 20);
    document.getElementById("canvas-1").onclick = function() {
      snek.setup()
      game.runSnek(this)
    };
    return
  },
  setup: function() {
    document.getElementById("blue").innerHTML = this.score;
    this.vDir = {
        x: 0,
        y: this.size
      }
      // for (let i = 0,len = this.pos.length; i < len; ++i) {
      //   this[i].pos = [{
      //     x: Math.round(canvas.width-this.size*10),
      //     y: 0
      //   }      
      // }
    this.pos.length = 0;
    this.pos = [{
      x: Math.round(canvas.width - this.size * 10),
      y: 0
    }, {
      x: Math.round(canvas.width - this.size * 10),
      y: 0
    }];
    // this.vDir = {
    //   x: -this.size,
    //   y: 0
    // }
    // this.pos = [{
    //   x: this.size*15,
    //   y: this.size*5
    // }, {
    //   x: this.size*15,
    //   y: this.size*5
    // }];
    this.addBlocks(13);
    this.draw();
  },
}

const wurm = { //red
  score: 0,
  size: game.size,
  pos: [],
  strokeStyle: '#000',
  fillStyle: '#aaa',
  draw: function() {
    // ctx.beginPath();
    // for (let i = 0, len = this.pos.length; i < len; ++i) {
    //   ctx.rect(this.pos[i].x, this.pos[i].y, this.size, this.size);
    // }
    // ctx.fillStyle = this.fillStyle;
    // ctx.fill();
    // ctx.strokeStyle = this.strokeStyle;
    // ctx.stroke();

    for (let i = this.pos.length - 1, c = 99994; i > 0; --i) {
      c--
      ctx.beginPath();
      ctx.rect(this.pos[i].x, this.pos[i].y, this.size, this.size);
      if (c % 6 > 2) {
        ctx.fillStyle = '#e00';
      } else {
        ctx.fillStyle = '#000';
      }
      ctx.fill();
      //ctx.strokeStyle = this.strokeStyle;
      //ctx.stroke();    
    }

  },
  velocity: {
    x: 0,
    y: 0
  },
  vDir: {
    x: 0,
    y: 0
  },
  input: function() {
    if ((keys[65]) && this.velocity.x != this.size) { //left
      this.vDir = {
        x: -this.size,
        y: 0
      }
    } else if ((keys[87]) && this.velocity.y != this.size) { //up
      this.vDir = {
        x: 0,
        y: -this.size
      }
    } else if ((keys[68]) && this.velocity.x != -this.size) { //right
      this.vDir = {
        x: this.size,
        y: 0
      }
    } else if ((keys[83]) && this.velocity.y != -this.size) { //down
      this.vDir = {
        x: 0,
        y: this.size
      }
    }
  },
  move: function() {
    if (this.velocity) {
      this.velocity.x = this.vDir.x;
      this.velocity.y = this.vDir.y;
      this.pos.shift(); //removes a block from the front
      this.pos.push({ //adds a new block to the end
        x: this.pos[this.pos.length - 1].x + this.velocity.x,
        y: this.pos[this.pos.length - 1].y + this.velocity.y
      })
    }
  },
  addBlocks: function(num) {
    for (let i = 0; i < num; ++i) {
      this.pos.unshift({
        x: this.pos[0].x,
        y: this.pos[0].y
      })
    }
  },
  hitGoal: function() {
    if (this.pos[this.pos.length - 1].x === goal.pos.x && this.pos[this.pos.length - 1].y === goal.pos.y) {
      goal.move();
      this.addBlocks(Math.ceil(this.pos.length * 0.1) + 10);
      this.score++;
      document.getElementById("red").innerHTML = this.score;
    }
  },
  collision: function() {
    let len = this.pos.length - 1;
    //hitting walls
    // if (this.pos[len].x < 0 || this.pos[len].x > canvas.width - this.size || this.pos[len].y < 0 || this.pos[len].y > canvas.height - game.size) {
    //   this.gameOver();
    // }
    //around the world
    if (this.pos[len].x < 0) this.pos[len].x = canvas.width - this.size;
    if (this.pos[len].x > canvas.width - this.size) this.pos[len].x = 0;
    if (this.pos[len].y < 0) this.pos[len].y = canvas.height - this.size;
    if (this.pos[len].y > canvas.height - this.size) this.pos[len].y = 0;

    for (let i = 0; i < len; ++i) { //head of snek hitting snek body
      if ((this.pos[len].x === this.pos[i].x && this.pos[len].y === this.pos[i].y)) {
        this.gameOver();
      }
    }
    for (let i = 0; i < snek.pos.length; ++i) { //head of snek hitting wurm body
      if ((this.pos[len].x === snek.pos[i].x && this.pos[len].y === snek.pos[i].y)) {
        this.gameOver();
      }
    }
  },
  gameOver: function() {
    this.score--;
    document.getElementById("red").innerHTML = this.score;
    game.run = false;
    ctx.fillStyle = '#f0f';
    ctx.fillRect(this.pos[this.pos.length - 1].x, this.pos[this.pos.length - 1].y, this.size, this.size);
    ctx.fillStyle = '#555';
    ctx.font = '20px sans-serif';
    ctx.fillText("click for sneks", canvas.width / 2, canvas.height - 20);
    document.getElementById("canvas-1").onclick = function() {
      snek.setup()
      wurm.setup()
      game.runSnek(this)
    };
    return
  },
  setup: function() {
    document.getElementById("red").innerHTML = this.score;
    this.vDir = {
      x: 0,
      y: this.size
    }
    this.pos = [{
      x: this.size * 10,
      y: 0
    }, {
      x: this.size * 10,
      y: 0
    }];
    this.addBlocks(13);
    this.draw();
  },
}