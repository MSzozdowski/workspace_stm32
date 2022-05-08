/*
 * parser_simple.h
 *
 *  Created on: Jan 8, 2022
 *      Author: Michal
 */

#ifndef INC_PARSER_COMPLEX_H_
#define INC_PARSER_COMPLEX_H_
#define ENDLINE '\n'

void Parser_TakeLine(RingBuffer_t *Buf, uint8_t *Destination);
void Parser_Parse(uint8_t *Data);
#endif /* INC_PARSER_COMPLEX_H_ */
