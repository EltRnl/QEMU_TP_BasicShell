/*
 * test.h
 *
 *  Created on: Jan 12, 2021
 *      Author: ogruber
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stddef.h>
#include <stdint.h>

/*
 * Board constantes such as BARs
 */
#include "board.h"

struct buffers {
	struct cb * rx;
	struct cb * tx;
	
};

#endif /* MAIN_H_ */
