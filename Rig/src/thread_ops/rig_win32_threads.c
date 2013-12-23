/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_win32_threads.c 895 2011-03-07 22:50:08Z llongi $
 */

#define RIG_THREAD_TYPE HANDLE WINAPI


/**
 * Have the calling thread relinquish the CPU, switching execution over to
 * another thread/process ready to run.
 */
void rig_thread_yield(void) {
	Sleep(0);
}
