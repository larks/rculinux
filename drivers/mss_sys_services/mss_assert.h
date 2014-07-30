/*******************************************************************************
 * (c) Copyright 2009 Actel Corporation.  All rights reserved.
 * 
 * Assertion implementation.
 *
 * This file provides the implementation of the ASSERT macro. This file can be
 * modified to cater for project specific requirements regarding the way
 * assertions are handled.
 *
 * SVN $Revision: 2959 $
 * SVN $Date: 2010-09-02 10:06:23 +0100 (Thu, 02 Sep 2010) $
 */
#ifndef __MSS_ASSERT_H_
#define __MSS_ASSERT_H_

#define ASSERT(x) do { if (!(x)) BUG(); } while (0)

#endif  /* __MSS_ASSERT_H_ */
