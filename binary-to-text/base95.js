
function Encoder(push){
	this.b=0;
	this.n=0;
	this.push=push;
}
Encoder.prototype.data = function(data){
	var i,n=data.length;
	for(i=0;i<n;++i){
		this.b |= data[i] << this.n;
		this.n += 8;
		while(this.n>=14)
			this.encode();
	}
};
Encoder.prototype.end = function(){
	if(this.n>0)
		this.encode();
};
Encoder.prototype.peek = function(bits){
	return this.b & ((1<<bits)-1);
};
Encoder.prototype.rem = function(bits){
	bits = Math.min(bits,this.n);
	this.b >>= bits;
	this.n -= bits;
};
Encoder.prototype.encode = function(){
	var d = 0;
	if(this.peek(13)<833){
		d = this.peek(14);
		this.rem(14);
	}else{
		d = this.peek(13);
		this.rem(13);
	}
	this.push((d%95)+32);
	d-=d%95;
	this.push((d/95)+32);
};

function Decoder(push){
	this.v=-1;
	this.b=0;
	this.n=0;
	this.push=push;
}
Decoder.prototype.data = function(data){
	var i,n=data.length;
	var b1;
	for(i=0;i<n;++i){
		if(data[i]<32)continue;
		b1 = data[i]-32;
		if(b1>=95)continue;
		if(this.v<0)
			this.v = b1;
		else{
			this.v += b1*95;
			this.b |= this.v << this.n;
			if((this.v&8191)<833) this.n += 14;
			else this.n += 13;
			this.v = -1;
			while(this.n>=8){
				this.push(this.peek(8));
				this.rem(8);
			}
		}
	}
};
Decoder.prototype.end = function(){
	while(this.n>=8){
		this.push(this.peek(8));
		this.rem(8);
	}
};
Decoder.prototype.peek = function(bits){
	return this.b & ((1<<bits)-1);
};
Decoder.prototype.rem = function(bits){
	bits = Math.min(bits,this.n);
	this.b >>= bits;
	this.n -= bits;
};



//var v = new Encoder(function(it){ console.log(String.fromCharCode(it)); });
/*
var v = new Encoder(function(it){ console.log(it); });
v.data(new Buffer("hallo"));
v.end();
*/
//*
var dec = new Decoder(function(it){ console.log(it); });
//var dec = new Decoder(function(it){ console.log(String.fromCharCode(it)); });
//var dec = new Decoder(function(it){ console.log(String.fromCharCode(it)); });
//var enc = new Encoder(function(it){ console.log('>',it); dec.data([it]); });
var enc = new Encoder(function(it){ console.log('>',String.fromCharCode(it)); dec.data([it]); });
//var enc = new Encoder(function(it){ dec.data([it]); });
//enc.data(new Buffer("hallo"));
//for(var i=0;i<256;++i)
//	enc.data([i]);
//for(var i=128;i<224;++i)enc.data([i]);
for(var i=0;i<256;++i)enc.data([i]);
//for(var i=0;i<128;++i)enc.data([i]);
enc.end();
dec.end();
//*/

