var can, ctx;
var backgroundColor = "#aabbdd";
var points = [];

document.generatePointCount = 5;
document.maxStep = 0.1;
document.stepFactor = 0.20;
document.precision = 0.1;
var scale = 0.8;

class point{
	constructor(_x,_y,_p1,_p2){
		this.x = _x;
		this.y = _y;
		this.p1 = undefined;
		this.p2 = undefined;
		this.objective1 = undefined;	
		this.objective2 = undefined;
		this.useObjective1 = true;
	}

	set_points(_p1, _p2){
		this.p1 = _p1;
		this.p2 = _p2;
	}

	getObjective(){
		if(this.useObjective1){
			return this.objective1;
		}else{
			return this.objective2;
		}
	}

	generateObjective(){
		const TO_RADIAN = Math.PI/180;
		const OFFSET = 60*TO_RADIAN;
		let d = Math.sqrt((this.p1.x-this.p2.x)**2+(this.p1.y-this.p2.y)**2);
		let alpha;
		if(this.p1.x!=this.p2.x){
			alpha = Math.atan2( this.p2.y-this.p1.y, this.p2.x-this.p1.x );
		}else{
			console.log("E")
			alpha = 90*TO_RADIAN*Math.sign(this.p2.y-this.p1.y); // Potential error
		}
		let x1 = this.p1.x + d * Math.cos(alpha+OFFSET);
		let y1 = this.p1.y + d * Math.sin(alpha+OFFSET);
		let x2 = this.p1.x + d * Math.cos(alpha-OFFSET);
		let y2 = this.p1.y + d * Math.sin(alpha-OFFSET);
		this.useObjective1 = ((this.x-x1)**2 + (this.y-y1)**2 < (this.x-x2)**2 + (this.y-y2)**2 )
		this.objective1 = [x1, y1];
		this.objective2 = [x2, y2]
	}



	step(){
		let objective = this.getObjective();
		let s = Math.min(document.stepFactor, Math.sqrt((objective[0]-this.x)**2+(objective[1]-this.y)**2)/document.maxStep);
		this.x += (objective[0]-this.x)*s;
		this.y += (objective[1]-this.y)*s;
	}

	draw2d(c){
		function s(coord){return Math.floor(can.width*(coord*scale/2+0.5))}
		let x = s(this.x);
		let y = s(this.y);
		c.beginPath();
		c.arc(x,y,2, 0, Math.PI*2);
		c.fill();
		c.beginPath();
		c.moveTo(x,y);
		c.lineTo(s(this.p1.x),s(this.p1.y));
		c.moveTo(x,y);
		c.lineTo(s(this.p2.x),s(this.p2.y));
		c.moveTo(x,y);
		c.lineTo(s(this.getObjective()[0]), s(this.getObjective()[1]));
		c.stroke();
	}
}

function generatePoints(){
	points = [];	
	for(let i=0;i<document.generatePointCount;i++){
		points.push(new point(Math.random(),Math.random()));
	}
	points.forEach(function (element){ 	
		let p1,p2; 			
		while(p1==p2 || element == p1 || element == p2){
			p1=points[generatePointIndex()];
			p2=points[generatePointIndex()];
		};
		element.set_points(p1,p2);
	});
}
	
function shufflePoints(){
	points.forEach(function(element){
		element.x=Math.random(); 
		element.y=Math.random();
	})
}

function generatePointIndex(){
	return Math.floor(Math.random()*document.generatePointCount);
}

function loaded(){
	can = document.getElementById("mainCanvas");
	ctx = can.getContext("2d");
	can.width=400;
	can.height=400;
	generatePoints();
	frame();
}

function factorial(n){
	if(n>1) return n*factorial(n-1);
	return 1;
}

function combs(num){
	return ((factorial(num-1)/factorial(num-3)/2)**num);
}

function step(){

	points.forEach((element) => element.generateObjective());
	points.forEach(function(element1){
		points.forEach(function(element2){
			if(element1 == element2) return;
			let n = document.precision;
			let dx = Math.abs(element1.x-element2.x);
			let dy = Math.abs(element1.y-element2.y);
			if( dx < n && dy < n ){
				element1.x = 5*Math.random()-1;
				element1.y = 5*Math.random()-1;
				//element1.useObjective1 = !element1.useObjective1;
				//let obj = element1.getObjective();
				//element1.x = obj[0];
				//element1.y = obj[1];
				//element1.useObjective1 = !element1.useObjective1;
			}
		})});
	points.forEach((element) => element.step());
}


function frame(){
	ctx.fillStyle=backgroundColor;
	ctx.fillRect(0,0,can.width,can.height);
	ctx.fillStyle="#000000";
	ctx.strokeStyle="#00000020";
	step();
	let maxd = 0;
	let sumx = 0;
	let sumy = 0;
	points.forEach(function(element){
		maxd = Math.max(maxd, Math.sqrt(element.x**2+element.y**2)); 
		sumx += element.x; 
		sumy += element.y
	} );
	sumx /= maxd*document.generatePointCount;
	sumy /= maxd*document.generatePointCount;
	points.forEach(function(element){
		element.x/=maxd;
		element.y/=maxd;
		element.x-= sumx;
		element.y-= sumy;
		element.draw2d(ctx);
	})
	window.requestAnimationFrame(frame);
}
