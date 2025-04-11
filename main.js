var can, ctx;
var backgroundColor = "#aabbdd";
var points = [];
var frameCount = 0;

document.generatePointCount = 5;
document.maxStep = 0.1;
document.stepFactor = 0.20;
document.precision = 0.1;
var scale = 0.8;

class point{
	constructor(_x,_y,_p1,_p2){
		this.x = _x;
		this.y = _y;
		this.p1 = _p1;
		this.p2 = _p2;
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
		let p1 = points[this.p1];
		let p2 = points[this.p2];
		let d = Math.sqrt((p1.x-p2.x)**2+(p1.y-p2.y)**2);
		let alpha;
		if(p1.x!=p2.x){
			alpha = Math.atan2( p2.y-p1.y, p2.x-p1.x );
		}else{
			alpha = 90*TO_RADIAN*Math.sign(p2.y-p1.y); // Potential error
		}
		let x1 = p1.x + d * Math.cos(alpha+OFFSET);
		let y1 = p1.y + d * Math.sin(alpha+OFFSET);
		let x2 = p1.x + d * Math.cos(alpha-OFFSET);
		let y2 = p1.y + d * Math.sin(alpha-OFFSET);
		this.useObjective1 = ((this.x-x1)**2 + (this.y-y1)**2) < ((this.x-x2)**2 + (this.y-y2)**2 )
		this.objective1 = [x1, y1];
		this.objective2 = [x2, y2];
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
		c.strokeStyle="#00bb00";
		c.beginPath();
		c.moveTo(x,y);
		c.lineTo(s(points[this.p1].x),s(points[this.p1].y));
		c.moveTo(x,y);
		c.lineTo(s(points[this.p2].x),s(points[this.p2].y));
		c.stroke();
		c.strokeStyle="#bb0000";
		c.beginPath();
		c.moveTo(x,y);
		c.lineTo(s(this.getObjective()[0]), s(this.getObjective()[1]));
		c.stroke();
	}
}

function generatePoints(){
	points = [];	
	let p1,p2;
	document.currentOrder = generatePointOrder(document.generatePointCount);
	let dm = generateDistanceMatrix(document.currentOrder);
	for(let i=0;i<document.generatePointCount;i++){
		[p1, p2] = document.currentOrder._data[i];
		points.push(new point(Math.random(),Math.random(),p1,p2));
	}
	let table = document.getElementById("sumTable");
	let solvable = document.getElementById("solvable");
	let result = solvePoints(dm);
	solvable.innerText = (result.solvable ? "Solvable" : (result.solvable==false) ? "Unsolvable" : "Undecided").toString();
	if(result.solvable){
		result.solution.forEach(function(p,i){[points[i].x,points[i].y]=p;})
	}
}

function factorial(n){
	if(n<2){ return 1};
	return n*factorial(n-1);
}

function combinations(n,r){
	return factorial(n)/factorial(n-r)/factorial(r);
}

function getDistanceMatrixIndex(pair, n){
	if(pair[0]>=pair[1]){
		if(pair[0]==pair[1]){return NaN}
		pair = [pair[1], pair[0]];
	}
	if(pair[1] >= n){return NaN}
	return pair[0]*(2*n-3-pair[0])/2-1+pair[1];
}

function generateDistanceMatrix(order){
	let n = order.size()[0];
	if(n>800){
		alert("Whoa buddy, that's gonna crash your browser, calm down!")
		return NaN;
	}
	let out = math.zeros(n,n*(n+1)/2+1);
	order.rows().forEach(function(value, index){
		data = value._data[0];
		out.set([index,data[0]],1);
		out.set([index,data[1]],1);
		out.set([index,n+getDistanceMatrixIndex(data,n)],2);
	})
	return out;
}

function solveDistanceMatrix(input){
	const [height, width] = input.size();
	let data = input._data;
	for(let x=0;x<width;x++){
		for(let y=0;y<height;y++){
			if(data[y][x]==0){continue}
			if(data[y].findIndex((e)=>{return e!=0})<x){continue}
			data[y]=math.multiply(data[y],1/data[y][x]);
			for(let i = 0; i<height;i++){
				if(data[i][x]==0){continue}
				let res = math.multiply(data[y][x] / data[i][x],data[y]);
				if (res.toString()!=data[i].toString()){
					data[i]=math.subtract(data[i],res);
				}
			}
		}
	}
	return math.matrix(data.sort().reverse());
}

function solvePoints(input){
	const [height, width] = input.size();
	const data = input._data;
	
	let result = new Set();
	result.solvable = false;
	let count;
	if(data.every(function(e1){
		count = 0;
		data.forEach(function(e2){
			count++;
			for(let i=0; i<width;i++){
				if(e1[i]!=e2[i]){
					count--;
					break;
				}
			}	
		});
		if(count > 2){return false}
		if(count == 2){
			
		}
		return true;
	})==false){return result}


	input = solveDistanceMatrix(input);

	let solution = new Array(height);
	for(let x=0; x<height; x++){
		let resolved = false;
		let needsToResolve = true;
		let px = Math.random();
		let py = Math.random();
		for(let y=0; y<height;y++){
			if(input._data[y][x]==0){continue}
			for(let i=x+1; i<height; i++){
				if(input._data[y][height+getDistanceMatrixIndex([x,i],height)]!=0 || input._data[y][i]!=0){
					needsToResolve=false;
				}
			}
			if(needsToResolve){
				let first = [0,0];
				let second = [0,0];
				for(let i=0; i<x; i++){
					
					let f = input._data[y][height+getDistanceMatrixIndex([x,i],height)];
					let s = input._data[y][i];
					first[0]+= solution[i][0] * f;
					first[1]+= solution[i][1] * f;
					second[0]+= solution[i][0]**2 * s;
					second[1]+= solution[i][1]**2 * s;
				}
				let ppx = 0;
				let ppy = 0;
				if(second[0]!=0){
				}
					ppx = first[0]/2/second[0]+Math.sqrt(first[0]**2/4-input._data[x][width-1]); // TODO, make non-naive, only positive at the moment;
				if(second[1]!=0){
					ppy = first[1]/2/second[1]+Math.sqrt(first[1]**2/4-input._data[1][width-1]); // TODO, make non-naive, only positive at the moment;
				}
				if(resolved && (px!=ppx || py!=ppy)){
					console.log("Impossible, since",px," != ", ppx, " or ", py, " != ",ppy);
					return result;
				}
				px=ppx;
				py=ppy;
				resolved=true;
				input._data[x][width-1] += px**2 + py**2;
				for(let i=x+1; i<height; i++){
					input._data[x][width-1] = input._data[x][i]*solution[2] 
				}
			}

		}
		solution[x]=[px,py];
	}
	
	result.solvable = solution.every(function(e1,i1){
		return solution.every(function(e2,i2){
			if(e1[0]==e2[0] && e1[1]==e2[1] && i1!=i2){return false}
			return true;
		})
	});
	result.solution = solution;
	return result;
}

function generatePointOrder(n){
	let out = math.zeros(n,2)
	for (let i=0;i<n;i++){
		let p1,p2;
		while(p1==p2 || i==p1 || i==p2){
			p1 = Math.floor(Math.random()*n);
			p2 = Math.floor(Math.random()*n);
		}
		out.set([i,0],p1);
		out.set([i,1],p2)
	}
	return out;
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
	frameCount++;
	window.requestAnimationFrame(frame);
}
