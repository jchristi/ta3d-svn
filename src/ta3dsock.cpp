/*  TA3D, a remake of Total Annihilation
    Copyright (C) 2006  Roland BROCHARD

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA*/

#include "stdafx.h"
#include "TA3D_NameSpace.h"

/******************************/
/**  methods for TA3DSock  ****/
/******************************/


int TA3DSock::Open(const char* hostname,const char* port){

	tcpsock.Open(hostname,port,PROTOCOL_TCPIP);

	if(!tcpsock.isOpen())
		return -1;

	return 0;

}



int TA3DSock::Accept(TA3DSock** sock){
	int v;
	(*sock) = new TA3DSock;
	v = tcpsock.Accept((*sock)->tcpsock);
	if(v<0){
		//accept error
		delete (*sock);
		return -1;
	}

	if(!(*sock)->tcpsock.isOpen() ){
		delete *sock;
		return -1;
	}

	return 0;
}

int TA3DSock::Accept(TA3DSock** sock,int timeout){
	int v;
	*sock = new TA3DSock;
	v = tcpsock.Accept((*sock)->tcpsock,timeout);
	
	if(v<0){
		//accept error
		delete (*sock);
		return -1;
	}

	
	if(!(*sock)->tcpsock.isOpen() ){
		delete *sock;
		return -1;
	}

	return 0;
}

int TA3DSock::isOpen(){
	return tcpsock.isOpen();
}

void TA3DSock::Close(){
	if( tcpsock.isOpen() )
		tcpsock.Close();
}



//byte shuffling
void TA3DSock::putLong(uint32_t x){//uint32
	uint32_t temp;
	temp = x;
	memcpy(outbuf+obp,&temp,4);
	obp += 4;
}

void TA3DSock::putShort(uint16_t x){//uint16
	uint16_t temp;
	temp = x;
	memcpy(outbuf+obp,&temp,2);
	obp += 2;
}

void TA3DSock::putByte(uint8_t x){//uint8
	memcpy(outbuf+obp,&x,1);
	obp += 1;
}

void TA3DSock::putString(const char* x){//null terminated
	int n = strlen(x);
	if(n < TA3DSOCK_BUFFER_SIZE - obp - 1){
		memcpy(outbuf+obp,x,n);
		obp+=n;
	}
	else{
		memcpy(outbuf+obp,x,TA3DSOCK_BUFFER_SIZE - obp - 1);
		obp+=TA3DSOCK_BUFFER_SIZE - obp - 1;
	}
	putByte('\0');
}	

void TA3DSock::putFloat(float x){
	float temp;
	temp = nlSwapf(x);
	memcpy(outbuf+obp,&temp,4);
	obp += 4;
}

uint32 TA3DSock::getLong()	//uint32
{
	uint32 result = *((uint32*)(tcpinbuf+tibrp));
	tibrp += 4;
	return result;
}

uint16 TA3DSock::getShort()	//uint16
{
	uint16 result = *((uint16*)(tcpinbuf+tibrp));
	tibrp += 2;
	return result;
}

byte TA3DSock::getByte()	//uint8
{
	byte result = *((byte*)(tcpinbuf+tibrp));
	tibrp ++;
	return result;
}

void TA3DSock::getString(char* x)	//null terminated
{
	while( *x = *((char*)(tcpinbuf+tibrp)) ) {
		tibrp++;
		x++;
		}
	tibrp++;
}

void TA3DSock::getBuffer(char* x, int size)
{
	memcpy( x, tcpinbuf + tibrp, size );
	tibrp += size;
}

float TA3DSock::getFloat()
{
	float result = *((float*)(tcpinbuf+tibrp));
	tibrp += 4;
	return result;
}


void TA3DSock::sendTCP(byte *data, int size)
{
	tcpmutex.Lock();

	int n = 0;
	int count = 0;
	while( !n && count < 100 && isOpen() ) {
		n = tcpsock.Send(data,size);
		count++;
		}

	tcpmutex.Unlock();
}

void TA3DSock::sendTCP(){
	tcpmutex.Lock();

	int n = 0;
	int count = 0;
	while( !n && count < 100 && isOpen() ) {
		n = tcpsock.Send(outbuf,obp);
		count++;
		}
	obp = 0;

	tcpmutex.Unlock();
}

void TA3DSock::recvTCP(){
	if( tiremain == 0 )	return;

	int p = tcpsock.Recv( tcpinbuf, TA3DSOCK_BUFFER_SIZE );
	if( p <= 0 ) {
		rest(1);
		tiremain = -1;
		return;
		}
	tiremain = 0;
	tibp = p;
}


void TA3DSock::pumpIn(){
	recvTCP();
}

char TA3DSock::getPacket(){
	if(tiremain != 0)
		return 0;
	else
		return tcpinbuf[0];
}

void TA3DSock::cleanPacket(){
	if(tiremain<=0) {
		tcpinbuf[tibp] = 0;
		printf("tcpinbuf = '%s'\n", tcpinbuf);
		tibp = 0;
		tiremain = -1;
	}
}



int TA3DSock::takeFive(int time){
	return tcpsock.takeFive( time );
}


int TA3DSock::sendSpecial(struct chat* chat, bool all){
	tcpmutex.Lock();
	if( all )
		putByte('A');
	else
		putByte('X');
	putShort(chat->from);
	putString(chat->message);
	sendTCP();
	tcpmutex.Unlock();
	return 0;
}

int TA3DSock::sendChat(struct chat* chat){
	tcpmutex.Lock();
	putByte('C');
	putShort(chat->from);
	putString(chat->message);
	sendTCP();
	tcpmutex.Unlock();
	return 0;
}

int TA3DSock::sendOrder(struct order* order){
	tcpmutex.Lock();
	putByte('O');
	putLong(order->timestamp);
	putLong(order->unit);
	putByte(order->command);
	putFloat(order->x);
	putFloat(order->y);
	putLong(order->target);
	putByte(order->additional);
	sendTCP();
	tcpmutex.Unlock();
	return 0;
}

int TA3DSock::sendSync(struct sync* sync){
	tcpmutex.Lock();

	putByte('S');
	putLong(sync->timestamp);
	putShort(sync->unit);
	putFloat(sync->x);
	putFloat(sync->y);
	putFloat(sync->z);
	
	if( sync->hp ) {			// Unit sync
		putShort(sync->hp);
		putFloat(sync->vx);
		putFloat(sync->vz);
		putShort(sync->orientation);
		putByte(sync->build_percent_left);
		}
	else {						// Weapon sync
		putShort(sync->hp);
		putFloat(sync->vx);
		putFloat(sync->vy);
		putFloat(sync->vz);
		}
	sendTCP();

	tcpmutex.Unlock();
	return 0;
}

int TA3DSock::sendPing(){
	tcpmutex.Lock();
	putByte('P');
	putByte(0);
	sendTCP();
	tcpmutex.Unlock();
}

int TA3DSock::sendEvent(struct event* event){
	tcpmutex.Lock();
	putByte('E');
	putByte(event->type);
	switch( event->type )
	{
	case EVENT_DRAW:
		putFloat(event->x);
		putFloat(event->y);
		putFloat(event->z);
		putLong(event->opt3);
		putString((const char*)(event->str));
		break;
	case EVENT_PRINT:
		putFloat(event->x);
		putFloat(event->y);
		putString((const char*)(event->str));
		break;
	case EVENT_PLAY:
		putString((const char*)(event->str));
		break;
	case EVENT_CLS:
	case EVENT_CLF:
	case EVENT_INIT_RES:
		break;
	case EVENT_CAMERA_POS:
		putShort(event->opt1);
		putFloat(event->x);
		putFloat(event->z);
		break;
	case EVENT_UNIT_SYNCED:
		putShort(event->opt1);
		putShort(event->opt2);
		putLong(event->opt3);
		break;
	case EVENT_UNIT_DAMAGE:
		putShort(event->opt1);
		putShort(event->opt2);
		break;
	case EVENT_WEAPON_CREATION:
		putShort(event->opt1);
		putShort(event->opt2);
		putFloat(event->x);
		putFloat(event->y);
		putFloat(event->z);
		putLong(((real32*)(event->str))[0]);
		putLong(((real32*)(event->str))[1]);
		putLong(((real32*)(event->str))[2]);
		putLong(((sint16*)(event->str))[6]);
		putLong(((sint16*)(event->str))[7]);
		putLong(((sint16*)(event->str))[8]);
		putLong(((sint16*)(event->str))[9]);
		break;
	case EVENT_UNIT_SCRIPT:
		putShort(event->opt1);
		putShort(event->opt2);
		putLong(event->opt3);
		putLong(event->opt4);
		for( int i = 0 ; i < event->z ; i++ )
			putLong(((sint32*)(event->str))[i]);
		break;
	case EVENT_UNIT_DEATH:
		putShort(event->opt1);
		break;
	case EVENT_UNIT_CREATION:
		printf("sending unit creation event (%s)\n", event->str);
		putShort(event->opt1);
		putShort(event->opt2);
		putFloat(event->x);
		putFloat(event->z);
		putString((const char*)(event->str));
		break;
	};
	sendTCP();
	tcpmutex.Unlock();
	return 0;
}


int TA3DSock::makeSpecial(struct chat* chat){
	if(tcpinbuf[0] != 'X' && tcpinbuf[0] != 'A'){
		Console->AddEntry("makeSpecial error: the data doesn't start with a 'X' or a 'A'");
		return -1;
	}
	if(tiremain == -1){
		return -1;
	}
	tibrp = 1;
	chat->from = getShort();
	getBuffer(chat->message,253);
	(chat->message)[252] = '\0';
	tibp = 0;
	tiremain = -1;

	return 0;
}

int TA3DSock::makeChat(struct chat* chat){
	if(tcpinbuf[0] != 'C'){
		Console->AddEntry("makeChat error: the data doesn't start with a 'C'");
		return -1;
	}
	if(tiremain == -1){
		return -1;
	}
	tibrp = 1;
	chat->from = getShort();
	getBuffer(chat->message,253);
	(chat->message)[252] = '\0';
	tibp = 0;
	tiremain = -1;

	return 0;
}

int TA3DSock::makePing(){
	if(tcpinbuf[0] != 'P'){
		Console->AddEntry("makePing error: the data doesn't start with a 'P'");
		return -1;
	}
	if(tiremain == -1){
		return -1;
	}
	tibp = 0;
	tiremain = -1;

	return 0;
}

int TA3DSock::makeOrder(struct order* order){
	if(tcpinbuf[0] != 'O'){
		Console->AddEntry("makeOrder error: the data doesn't start with an 'O'");
		return -1;
	}
	if(tiremain == -1){
		return -1;
	}
	uint32_t temp;

	memcpy(&temp,tcpinbuf+1,4);
	order->timestamp = temp;

	memcpy(&temp,tcpinbuf+5,4);
	order->unit = nlSwapl(temp);

	order->command = tcpinbuf[9];

	memcpy(&temp,tcpinbuf+10,4);
	order->x = (float)nlSwapl(temp);

	memcpy(&temp,tcpinbuf+14,4);
	order->y = (float)nlSwapl(temp);

	memcpy(&temp,tcpinbuf+18,4);
	order->target = nlSwapl(temp);

	order->additional = tcpinbuf[19];
	
	tibp = 0;
	tiremain = -1;

	return 0;
}

int TA3DSock::makeSync(struct sync* sync){
	if(tcpinbuf[0] != 'S'){
		Console->AddEntry("makeSync error: the data doesn't start with an 'S'");
		return -1;
	}
	if(tiremain == -1){
		return -1;
	}
	tibrp = 1;
	
	sync->timestamp = getLong();
	sync->unit = getShort();
	sync->x = getFloat();
	sync->y = getFloat();
	sync->z = getFloat();

	sync->hp = getShort();

	if( sync->hp ) {		// Unit sync packet
		sync->vx = getFloat();
		sync->vz = getFloat();
		sync->orientation = getShort();
		sync->build_percent_left = getByte();
		}
	else {					// Weapon sync
		sync->vx = getFloat();
		sync->vy = getFloat();
		sync->vz = getFloat();
		}

	tibp = 0;
	tiremain = -1;

	return 0;
}

int TA3DSock::makeEvent(struct event* event){
	if(tcpinbuf[0] != 'E'){
		Console->AddEntry("makeEvent error: the data doesn't start with an 'E'");
		return -1;
	}
	if(tiremain == -1){
		return -1;
	}
	tibrp = 1;
	event->type = getByte();

	switch( event->type )
	{
	case EVENT_DRAW:
		event->x = getFloat();
		event->y = getFloat();
		event->z = getFloat();
		event->opt3 = getLong();
		getBuffer((char*)(event->str),24);
		break;
	case EVENT_PRINT:
		event->x = getFloat();
		event->y = getFloat();
		getBuffer((char*)(event->str),24);
		break;
	case EVENT_PLAY:
		getBuffer((char*)(event->str),24);
		break;
	case EVENT_CLS:
	case EVENT_CLF:
	case EVENT_INIT_RES:
		break;
	case EVENT_CAMERA_POS:
		event->opt1 = getShort();
		event->x = getFloat();
		event->z = getFloat();
		break;
	case EVENT_UNIT_SYNCED:
		event->opt1 = getShort();
		event->opt2 = getShort();
		event->x = getLong();
		break;
	case EVENT_UNIT_DAMAGE:
		event->opt1 = getShort();
		event->opt2 = getShort();
		break;
	case EVENT_WEAPON_CREATION:
		event->opt1 = getShort();
		event->opt2 = getShort();
		event->x = getLong();
		event->y = getLong();
		event->z = getLong();
		((sint32*)(event->str))[0] = getLong();
		((sint32*)(event->str))[1] = getLong();
		((sint32*)(event->str))[2] = getLong();
		((sint16*)(event->str))[6] = getLong();
		((sint16*)(event->str))[7] = getLong();
		((sint16*)(event->str))[8] = getLong();
		((sint16*)(event->str))[9] = getLong();
		break;
	case EVENT_UNIT_SCRIPT:
		event->opt1 = getShort();
		event->opt2 = getShort();
		event->x = getLong();
		event->z = getLong();
		for( int i = 0 ; i < event->z ; i++ )
			((sint32*)(event->str))[i] = getLong();
		break;
	case EVENT_UNIT_DEATH:
		event->opt1 = getShort();
		break;
	case EVENT_UNIT_CREATION:
		event->opt1 = getShort();
		event->opt2 = getShort();
		event->x = getLong();
		event->z = getLong();
		getBuffer((char*)(event->str),24);
		break;
	};

	tibp = 0;
	tiremain = -1;
	return 0;
}

int TA3DSock::getFilePort()				// For file transfer, first call this one to get the port which allows us to grab the right thread and buffer
{
	if( tcpinbuf[0] != 'F' && tcpinbuf[0] != 'R' ) {
		Console->AddEntry("getFilePort error: the data doesn't start with an 'F' or an 'R'");
		return -1;
		}
	if(tiremain == -1)
		return -1;
	return *((uint16*)(tcpinbuf+1));
}

int TA3DSock::getFileData(byte *buffer)	// Fill the buffer with the data and returns the size of the paquet
{
	if( tcpinbuf[0] != 'F' && tcpinbuf[0] != 'R' ) {
		Console->AddEntry("getFilePort error: the data doesn't start with an 'F' or an 'R'");
		return -1;
		}
	if(tiremain == -1)
		return -1;
	int size = tibp - 3;
	if( buffer )
		memcpy( buffer, tcpinbuf + 3, size );

	tibp = 0;
	tiremain = -1;
	
	return size;
}

