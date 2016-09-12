
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <op25_imbe_frame.h>

#ifdef DEBUG
/*
 * Convert bit vector to hex dump format and print
 */
static inline void
dump_cw(const_bit_vector cw, int len, FILE* fp)  // len in bytes
{
    int i, j;
    for (i = 0; i < len; i++)
    {
        int p = 0;
        for (j = 0; j <  8; j++)
        {
            p = (p << 1) + cw[ i*8 + j ];
        }
        fprintf(fp, "%02x ", p);
        if (!((i+1) % 16))
            fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
}
#endif // DEBUG

// this table used for HDU and also for TDU codeword mapping.
// FIXME: possible dup of GOLAY_CODEWORDS in hdu.cc ?
static const uint16_t hdu_codeword_bits[658] = {   // 329 symbols = 324 + 5 pad
    114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 
    130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 144, 145, 146, 147, 
    148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 
    164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 
    196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 
    212, 213, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 
    230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 
    246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 
    262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 
    278, 279, 280, 281, 282, 283, 284, 285, 288, 289, 290, 291, 292, 293, 294, 295, 
    296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 
    312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 
    328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 
    344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 360, 361, 
    362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 
    378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 
    394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 
    410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 
    426, 427, 428, 429, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 
    444, 445, 446, 447, 448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 
    460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 
    476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 
    492, 493, 494, 495, 496, 497, 498, 499, 500, 501, 504, 505, 506, 507, 508, 509, 
    510, 511, 512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 
    526, 527, 528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 
    542, 543, 544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 
    558, 559, 560, 561, 562, 563, 564, 565, 566, 567, 568, 569, 570, 571, 572, 573, 
    576, 577, 578, 579, 580, 581, 582, 583, 584, 585, 586, 587, 588, 589, 590, 591, 
    592, 593, 594, 595, 596, 597, 598, 599, 600, 601, 602, 603, 604, 605, 606, 607, 
    608, 609, 610, 611, 612, 613, 614, 615, 616, 617, 618, 619, 620, 621, 622, 623, 
    624, 625, 626, 627, 628, 629, 630, 631, 632, 633, 634, 635, 636, 637, 638, 639, 
    640, 641, 642, 643, 644, 645, 648, 649, 650, 651, 652, 653, 654, 655, 656, 657, 
    658, 659, 660, 661, 662, 663, 664, 665, 666, 667, 668, 669, 670, 671, 672, 673, 
    674, 675, 676, 677, 678, 679, 680, 681, 682, 683, 684, 685, 686, 687, 688, 689, 
    690, 691, 692, 693, 694, 695, 696, 697, 698, 699, 700, 701, 702, 703, 704, 705, 
    706, 707, 708, 709, 710, 711, 712, 713, 714, 715, 716, 717, 720, 721, 722, 723, 
    724, 725, 726, 727, 728, 729, 730, 731, 732, 733, 734, 735, 736, 737, 738, 739, 
    740, 741, 742, 743, 744, 745, 746, 747, 748, 749, 750, 751, 752, 753, 754, 755, 
    756, 757, 758, 759, 760, 761, 762, 763, 764, 765, 766, 767, 768, 769, 770, 771, 
    772, 773, 774, 775, 776, 777, 778, 779, 780, 781, 782, 783, 784, 785, 786, 787, 
    788, 789 };

static const uint32_t gly23127DecTbl[2048] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 147459, 
    1, 2, 2, 3, 2, 3, 3, 4268035, 2, 3, 3, 1574915, 3, 2097155, 294915, 4099, 
    1, 2, 2, 3, 2, 3, 3, 147459, 2, 3, 3, 147459, 3, 147459, 147459, 147458, 
    2, 3, 3, 32771, 3, 2051, 3149827, 786435, 3, 274435, 4194307, 2162691, 589827, 5275651, 10243, 147459, 
    1, 2, 2, 3, 2, 3, 3, 2621443, 2, 3, 3, 8195, 3, 1118211, 294915, 4196355, 
    2, 3, 3, 135171, 3, 2051, 294915, 1064963, 3, 4210691, 294915, 2162691, 294915, 663555, 294914, 294915, 
    2, 3, 3, 5505027, 3, 2051, 65539, 45059, 3, 557059, 6147, 2162691, 6299651, 262147, 1572867, 147459, 
    3, 2051, 548867, 2162691, 2051, 2050, 4325379, 2051, 1179651, 2162691, 2162691, 2162690, 20483, 2051, 294915, 2162691, 
    1, 2, 2, 3, 2, 3, 3, 2621443, 2, 3, 3, 327683, 3, 43011, 5242883, 4099, 
    2, 3, 3, 32771, 3, 1441795, 18435, 4099, 3, 4210691, 2236419, 4099, 589827, 4099, 4099, 4098, 
    2, 3, 3, 32771, 3, 4198403, 270339, 1116163, 3, 3145731, 6147, 4726787, 589827, 262147, 2129923, 147459, 
    3, 32771, 32771, 32770, 589827, 2121731, 4325379, 32771, 589827, 133123, 1327107, 32771, 589826, 589827, 589827, 4099, 
    2, 3, 3, 2621443, 3, 2621443, 2621443, 2621442, 3, 4210691, 6147, 1212419, 131075, 262147, 90115, 2621443, 
    3, 4210691, 1114115, 272387, 12291, 98307, 4325379, 2621443, 4210691, 4210690, 524291, 4210691, 3147779, 4210691, 294915, 4099, 
    3, 204803, 6147, 16387, 1097731, 262147, 4325379, 2621443, 6147, 262147, 6146, 6147, 262147, 262146, 6147, 262147, 
    2359299, 1576963, 4325379, 32771, 4325379, 2051, 4325378, 4325379, 40963, 4210691, 6147, 2162691, 589827, 262147, 4325379, 1056771, 
    1, 2, 2, 3, 2, 3, 3, 268291, 2, 3, 3, 8195, 3, 2097155, 5242883, 622595, 
    2, 3, 3, 32771, 3, 2097155, 655363, 1064963, 3, 2097155, 86019, 4587523, 2097155, 2097154, 10243, 2097155, 
    2, 3, 3, 32771, 3, 1581059, 65539, 6291459, 3, 4261891, 2883587, 1052675, 36867, 262147, 10243, 147459, 
    3, 32771, 32771, 32770, 4472835, 200707, 10243, 32771, 1179651, 540675, 10243, 32771, 10243, 2097155, 10242, 10243, 
    2, 3, 3, 8195, 3, 4358147, 65539, 1064963, 3, 8195, 8195, 8194, 542723, 262147, 2232323, 8195, 
    3, 851971, 6293507, 1064963, 12291, 1064963, 1064963, 1064962, 1179651, 38915, 524291, 8195, 4259843, 2097155, 294915, 1064963, 
    3, 2117635, 65539, 657411, 65539, 262147, 65538, 65539, 1179651, 262147, 4243459, 8195, 262147, 262146, 65539, 262147, 
    1179651, 4202499, 266243, 32771, 2654211, 2051, 65539, 1064963, 1179650, 1179651, 1179651, 2162691, 1179651, 262147, 10243, 4722691, 
    2, 3, 3, 32771, 3, 81923, 5242883, 139267, 3, 659459, 5242883, 2115587, 5242883, 262147, 5242882, 5242883, 
    3, 32771, 32771, 32770, 12291, 4720643, 2424835, 32771, 264195, 1122307, 524291, 32771, 180227, 2097155, 5242883, 4099, 
    3, 32771, 32771, 32770, 2230275, 262147, 544771, 32771, 24579, 262147, 196611, 32771, 262147, 262146, 5242883, 262147, 
    32771, 32770, 32770, 32769, 1048579, 32771, 32771, 32770, 6295555, 32771, 32771, 32770, 589827, 262147, 10243, 32771, 
    3, 1050627, 409603, 4263939, 12291, 262147, 34819, 2621443, 2195459, 262147, 524291, 8195, 262147, 262146, 5242883, 262147, 
    12291, 2228227, 524291, 32771, 12290, 12291, 12291, 1064963, 524291, 4210691, 524290, 524291, 12291, 262147, 524291, 198659, 
    4718595, 262147, 3153923, 32771, 262147, 262146, 65539, 262147, 262147, 262146, 6147, 262147, 262146, 262145, 262147, 262146, 
    83971, 32771, 32771, 32770, 12291, 262147, 4325379, 32771, 1179651, 262147, 524291, 32771, 262147, 262146, 2113539, 262147, 
    1, 2, 2, 3, 2, 3, 3, 1081347, 2, 3, 3, 327683, 3, 2097155, 536579, 4196355, 
    2, 3, 3, 135171, 3, 2097155, 18435, 786435, 3, 2097155, 4194307, 57347, 2097155, 2097154, 1245187, 2097155, 
    2, 3, 3, 2107395, 3, 4198403, 65539, 786435, 3, 557059, 4194307, 1052675, 1312771, 73731, 2129923, 147459, 
    3, 1130499, 4194307, 786435, 172035, 786435, 786435, 786434, 4194307, 133123, 4194306, 4194307, 20483, 2097155, 4194307, 786435, 
    2, 3, 3, 135171, 3, 286723, 65539, 4196355, 3, 557059, 3162115, 4196355, 131075, 4196355, 4196355, 4196354, 
    3, 135171, 135171, 135170, 5767171, 98307, 2105347, 135171, 75779, 1310723, 524291, 135171, 20483, 2097155, 294915, 4196355, 
    3, 557059, 65539, 16387, 65539, 3276803, 65538, 65539, 557059, 557058, 401411, 557059, 20483, 557059, 65539, 4196355, 
    2359299, 4202499, 1083395, 135171, 20483, 2051, 65539, 786435, 20483, 557059, 4194307, 2162691, 20482, 20483, 20483, 1056771, 
    2, 3, 3, 327683, 3, 4198403, 18435, 139267, 3, 327683, 327683, 327682, 131075, 1589251, 2129923, 327683, 
    3, 532483, 18435, 7340035, 18435, 98307, 18434, 18435, 1085443, 133123, 524291, 327683, 4464643, 2097155, 18435, 4099, 
    3, 4198403, 1703939, 16387, 4198403, 4198402, 2129923, 4198403, 24579, 133123, 2129923, 327683, 2129923, 4198403, 2129922, 2129923, 
    2359299, 133123, 77827, 32771, 1048579, 4198403, 18435, 786435, 133123, 133122, 4194307, 133123, 589827, 133123, 2129923, 1056771, 
    3, 1050627, 4235267, 16387, 131075, 98307, 1314819, 2621443, 131075, 2109443, 524291, 327683, 131074, 131075, 131075, 4196355, 
    2359299, 98307, 524291, 135171, 98307, 98306, 18435, 98307, 524291, 4210691, 524290, 524291, 131075, 98307, 524291, 1056771, 
    2359299, 16387, 16387, 16386, 534531, 4198403, 65539, 16387, 5308419, 557059, 6147, 16387, 131075, 262147, 2129923, 1056771, 
    2359298, 2359299, 2359299, 16387, 2359299, 98307, 4325379, 1056771, 2359299, 133123, 524291, 1056771, 20483, 1056771, 1056771, 1056770, 
    2, 3, 3, 4734979, 3, 2097155, 65539, 139267, 3, 2097155, 165891, 1052675, 2097155, 2097154, 278531, 2097155, 
    3, 2097155, 1318915, 67587, 2097155, 2097154, 4231171, 2097155, 2097155, 2097154, 524291, 2097155, 2097154, 2097153, 2097155, 2097154, 
    3, 393219, 65539, 1052675, 65539, 51203, 65538, 65539, 24579, 1052675, 1052675, 1052674, 4849667, 2097155, 65539, 1052675, 
    530435, 4202499, 2244611, 32771, 1048579, 2097155, 65539, 786435, 360451, 2097155, 4194307, 1052675, 2097155, 2097154, 10243, 2097155, 
    3, 1050627, 65539, 2392067, 65539, 528387, 65538, 65539, 4460547, 212995, 524291, 8195, 1089539, 2097155, 65539, 4196355, 
    49155, 4202499, 524291, 135171, 395267, 2097155, 65539, 1064963, 524291, 2097155, 524290, 524291, 2097155, 2097154, 524291, 2097155, 
    65539, 4202499, 65538, 65539, 65538, 65539, 65537, 65538, 2099203, 557059, 65539, 1052675, 65539, 262147, 65538, 65539, 
    4202499, 4202498, 65539, 4202499, 65539, 4202499, 65538, 65539, 1179651, 4202499, 524291, 280579, 20483, 2097155, 65539, 163843, 
    3, 1050627, 2101251, 139267, 819203, 139267, 139267, 139266, 24579, 4227075, 524291, 327683, 71683, 2097155, 5242883, 139267, 
    4390915, 282627, 524291, 32771, 1048579, 2097155, 18435, 139267, 524291, 2097155, 524290, 524291, 2097155, 2097154, 524291, 2097155, 
    24579, 2686979, 4458499, 32771, 1048579, 4198403, 65539, 139267, 24578, 24579, 24579, 1052675, 24579, 262147, 2129923, 526339, 
    1048579, 32771, 32771, 32770, 1048578, 1048579, 1048579, 32771, 24579, 133123, 524291, 32771, 1048579, 2097155, 397315, 4276227, 
    1050627, 1050626, 524291, 1050627, 6307843, 1050627, 65539, 139267, 524291, 1050627, 524290, 524291, 131075, 262147, 524291, 53251, 
    524291, 1050627, 524290, 524291, 12291, 98307, 524291, 4456451, 524290, 524291, 524289, 524290, 524291, 2097155, 524290, 524291, 
    167939, 1050627, 65539, 16387, 65539, 262147, 65538, 65539, 24579, 262147, 524291, 6422531, 262147, 262146, 65539, 262147, 
    2359299, 4202499, 524291, 32771, 1048579, 671747, 65539, 2103299, 524291, 69635, 524290, 524291, 4229123, 262147, 524291, 1056771, 
    1, 2, 2, 3, 2, 3, 3, 1081347, 2, 3, 3, 8195, 3, 4980739, 2164739, 4099, 
    2, 3, 3, 2375683, 3, 2051, 655363, 4099, 3, 229379, 4194307, 4099, 1073155, 4099, 4099, 4098, 
    2, 3, 3, 593923, 3, 2051, 270339, 6291459, 3, 3145731, 4194307, 296963, 36867, 73731, 1572867, 147459, 
    3, 2051, 4194307, 1187843, 2051, 2050, 114691, 2051, 4194307, 540675, 4194306, 4194307, 2490371, 2051, 4194307, 4099, 
    2, 3, 3, 8195, 3, 2051, 4214787, 458755, 3, 8195, 8195, 8194, 131075, 2146307, 1572867, 8195, 
    3, 2051, 1114115, 4751363, 2051, 2050, 2105347, 2051, 2625539, 1310723, 149507, 8195, 4259843, 2051, 294915, 4099, 
    3, 2051, 2260995, 16387, 2051, 2050, 1572867, 2051, 344067, 4329475, 1572867, 8195, 1572867, 2051, 1572866, 1572867, 
    2051, 2050, 266243, 2051, 2050, 2049, 2051, 2050, 40963, 2051, 4194307, 2162691, 2051, 2050, 1572867, 2051, 
    2, 3, 3, 4327427, 3, 81923, 270339, 4099, 3, 3145731, 573443, 4099, 131075, 4099, 4099, 4098, 
    3, 532483, 1114115, 4099, 6324227, 4099, 4099, 4098, 264195, 4099, 4099, 4098, 4099, 4098, 4098, 4097, 
    3, 3145731, 270339, 16387, 270339, 688131, 270338, 270339, 3145731, 3145730, 196611, 3145731, 4212739, 3145731, 270339, 4099, 
    151555, 4521987, 2623491, 32771, 1048579, 2051, 270339, 4099, 40963, 3145731, 4194307, 4099, 589827, 4099, 4099, 4098, 
    3, 299011, 1114115, 16387, 131075, 5251075, 34819, 2621443, 131075, 591875, 6553603, 8195, 131074, 131075, 131075, 4099, 
    1114115, 2228227, 1114114, 1114115, 802819, 2051, 1114115, 4099, 40963, 4210691, 1114115, 4099, 131075, 4099, 4099, 4098, 
    4718595, 16387, 16387, 16386, 2166787, 2051, 270339, 16387, 40963, 3145731, 6147, 16387, 131075, 262147, 1572867, 4292611, 
    40963, 2051, 1114115, 16387, 2051, 2050, 4325379, 2051, 40962, 40963, 40963, 917507, 40963, 2051, 2113539, 4099, 
    2, 3, 3, 8195, 3, 81923, 655363, 6291459, 3, 8195, 8195, 8194, 36867, 1181699, 278531, 8195, 
    3, 5246979, 655363, 67587, 655363, 303107, 655362, 655363, 264195, 540675, 3178499, 8195, 4259843, 2097155, 655363, 4099, 
    3, 393219, 1067011, 6291459, 36867, 6291459, 6291459, 6291458, 36867, 540675, 196611, 8195, 36866, 36867, 36867, 6291459, 
    2170883, 540675, 266243, 32771, 1048579, 2051, 655363, 6291459, 540675, 540674, 4194307, 540675, 36867, 540675, 10243, 1376259, 
    3, 8195, 8195, 8194, 3407875, 528387, 34819, 8195, 8195, 8194, 8194, 8193, 4259843, 8195, 8195, 8194, 
    49155, 2228227, 266243, 8195, 4259843, 2051, 655363, 1064963, 4259843, 8195, 8195, 8194, 4259842, 4259843, 4259843, 8195, 
    4718595, 1146883, 266243, 8195, 155651, 2051, 65539, 6291459, 2099203, 8195, 8195, 8194, 36867, 262147, 1572867, 8195, 
    266243, 2051, 266242, 266243, 2051, 2050, 266243, 2051, 1179651, 540675, 266243, 8195, 4259843, 2051, 2113539, 163843, 
    3, 81923, 2101251, 1835011, 81923, 81922, 34819, 81923, 264195, 4227075, 196611, 8195, 2629635, 81923, 5242883, 4099, 
    264195, 2228227, 4218883, 32771, 1048579, 81923, 655363, 4099, 264194, 264195, 264195, 4099, 264195, 4099, 4099, 4098, 
    4718595, 14339, 196611, 32771, 1048579, 81923, 270339, 6291459, 196611, 3145731, 196610, 196611, 36867, 262147, 196611, 526339, 
    1048579, 32771, 32771, 32770, 1048578, 1048579, 1048579, 32771, 264195, 540675, 196611, 32771, 1048579, 4333571, 2113539, 4099, 
    4718595, 2228227, 34819, 8195, 34819, 81923, 34818, 34819, 1069059, 8195, 8195, 8194, 131075, 262147, 34819, 8195, 
    2228227, 2228226, 1114115, 2228227, 12291, 2228227, 34819, 4456451, 264195, 2228227, 524291, 8195, 4259843, 1605635, 2113539, 4099, 
    4718594, 4718595, 4718595, 16387, 4718595, 262147, 34819, 1183747, 4718595, 262147, 196611, 8195, 262147, 262146, 2113539, 262147, 
    4718595, 2228227, 266243, 32771, 1048579, 2051, 2113539, 598019, 40963, 69635, 2113539, 5244931, 2113539, 262147, 2113538, 2113539, 
    2, 3, 3, 1081347, 3, 1081347, 1081347, 1081346, 3, 22531, 4194307, 2752515, 131075, 73731, 278531, 1081347, 
    3, 532483, 4194307, 67587, 331779, 4341763, 2105347, 1081347, 4194307, 1310723, 4194306, 4194307, 559107, 2097155, 4194307, 4099, 
    3, 393219, 4194307, 16387, 2637827, 73731, 137219, 1081347, 4194307, 73731, 4194306, 4194307, 73731, 73730, 4194307, 73731, 
    4194307, 2134019, 4194306, 4194307, 1048579, 2051, 4194307, 786435, 4194306, 4194307, 4194305, 4194306, 4194307, 73731, 4194306, 4194307, 
    3, 6356995, 788483, 16387, 131075, 528387, 2105347, 1081347, 131075, 1310723, 102403, 8195, 131074, 131075, 131075, 4196355, 
    49155, 1310723, 2105347, 135171, 2105347, 2051, 2105346, 2105347, 1310723, 1310722, 4194307, 1310723, 131075, 1310723, 2105347, 606211, 
    1060867, 16387, 16387, 16386, 4489219, 2051, 65539, 16387, 2099203, 557059, 4194307, 16387, 131075, 73731, 1572867, 2363395, 
    720899, 2051, 4194307, 16387, 2051, 2050, 2105347, 2051, 4194307, 1310723, 4194306, 4194307, 20483, 2051, 4194307, 163843, 
    3, 532483, 2101251, 16387, 131075, 2361347, 4784131, 1081347, 131075, 4227075, 1058819, 327683, 131074, 131075, 131075, 4099, 
    532483, 532482, 425987, 532483, 1048579, 532483, 18435, 4099, 2179075, 532483, 4194307, 4099, 131075, 4099, 4099, 4098, 
    100355, 16387, 16387, 16386, 1048579, 4198403, 270339, 16387, 790531, 3145731, 4194307, 16387, 131075, 73731, 2129923, 526339, 
    1048579, 532483, 4194307, 16387, 1048578, 1048579, 1048579, 2293763, 4194307, 133123, 4194306, 4194307, 1048579, 311299, 4194307, 4099, 
    131075, 16387, 16387, 16386, 131074, 131075, 131075, 16387, 131074, 131075, 131075, 16387, 131073, 131074, 131074, 131075, 
    4200451, 532483, 1114115, 16387, 131075, 98307, 2105347, 4456451, 131075, 1310723, 524291, 2131971, 131074, 131075, 131075, 4099, 
    16387, 16386, 16386, 16385, 131075, 16387, 16387, 16386, 131075, 16387, 16387, 16386, 131074, 131075, 131075, 16387, 
    2359299, 16387, 16387, 16386, 1048579, 2051, 561155, 16387, 40963, 69635, 4194307, 16387, 131075, 6815747, 329731, 1056771, 
    3, 393219, 2101251, 67587, 4204547, 528387, 278531, 1081347, 1638403, 4227075, 278531, 8195, 278531, 2097155, 278530, 278531, 
    49155, 67587, 67587, 67586, 1048579, 2097155, 655363, 67587, 143363, 2097155, 4194307, 67587, 2097155, 2097154, 278531, 2097155, 
    393219, 393218, 565251, 393219, 1048579, 393219, 65539, 6291459, 2099203, 393219, 4194307, 1052675, 36867, 73731, 278531, 526339, 
    1048579, 393219, 4194307, 67587, 1048578, 1048579, 1048579, 28675, 4194307, 540675, 4194306, 4194307, 1048579, 2097155, 4194307, 163843, 
    49155, 528387, 5373955, 8195, 528387, 528386, 65539, 528387, 2099203, 8195, 8195, 8194, 131075, 528387, 278531, 8195, 
    49154, 49155, 49155, 67587, 49155, 528387, 2105347, 4456451, 49155, 1310723, 524291, 8195, 4259843, 2097155, 1054723, 163843, 
    2099203, 393219, 65539, 16387, 65539, 528387, 65538, 65539, 2099202, 2099203, 2099203, 8195, 2099203, 5259267, 65539, 163843, 
    49155, 4202499, 266243, 3670019, 1048579, 2051, 65539, 163843, 2099203, 69635, 4194307, 163843, 794627, 163843, 163843, 163842, 
    2101251, 4227075, 2101250, 2101251, 1048579, 81923, 2101251, 139267, 4227075, 4227074, 2101251, 4227075, 131075, 4227075, 278531, 526339, 
    1048579, 532483, 2101251, 67587, 1048578, 1048579, 1048579, 4456451, 264195, 4227075, 524291, 1196035, 1048579, 2097155, 106499, 4099, 
    1048579, 393219, 2101251, 16387, 1048578, 1048579, 1048579, 526339, 24579, 4227075, 196611, 526339, 1048579, 526339, 526339, 526338, 
    1048578, 1048579, 1048579, 32771, 1048577, 1048578, 1048578, 1048579, 1048579, 69635, 4194307, 2367491, 1048578, 1048579, 1048579, 526339, 
    335875, 1050627, 2101251, 16387, 131075, 528387, 34819, 4456451, 131075, 4227075, 524291, 8195, 131074, 131075, 131075, 3211267, 
    49155, 2228227, 524291, 4456451, 1048579, 4456451, 4456451, 4456450, 524291, 69635, 524290, 524291, 131075, 26627, 524291, 4456451, 
    4718595, 16387, 16387, 16386, 1048579, 2138115, 65539, 16387, 2099203, 69635, 1343491, 16387, 131075, 262147, 4206595, 526339, 
    1048579, 69635, 141315, 16387, 1048578, 1048579, 1048579, 4456451, 69635, 69634, 524291, 69635, 1048579, 69635, 2113539, 163843 };

static const uint32_t hmg1063EncTbl[64] = {
    0, 12, 3, 15, 7, 11, 4, 8, 11, 7, 8, 4, 12, 0, 15, 3, 
    13, 1, 14, 2, 10, 6, 9, 5, 6, 10, 5, 9, 1, 13, 2, 14, 
    14, 2, 13, 1, 9, 5, 10, 6, 5, 9, 6, 10, 2, 14, 1, 13, 
    3, 15, 0, 12, 4, 8, 7, 11, 8, 4, 11, 7, 15, 3, 12, 0 };
static const uint32_t hmg1063DecTbl[16] = {
    0, 0, 0, 2, 0, 0, 0, 4, 0, 0, 0, 8, 1, 16, 32, 0 };

static const uint32_t rsGFexp[64] = {
    1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40, 
    19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37, 
    9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39, 
    13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0 };
static const uint32_t rsGFlog[64] = {
    63, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18, 
    4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56, 
    5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40, 
    10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58 };

int hmg1063Dec (uint32_t Dat, uint32_t Par)
{
    assert ((Dat < 64) && (Par < 16));
    return Dat ^ hmg1063DecTbl [ hmg1063EncTbl[Dat] ^ Par];
}

int
rsDec(int nroots, int FirstInfo, uint8_t HB[])
{
    //RS (63,63-nroots,nroots+1) decoder where nroots = number of parity bits
    // rsDec(8, 39) rsDec(16, 27) rsDec(12, 39)

    int lambda[18]; //Err+Eras Locator poly
    int S[17];      //syndrome poly
    int b[18];
    int t[18];
    int omega[18];
    int root[17];
    int reg[18];
    int locn[17];

    int i,j, count, r, el, SynError, DiscrR, q, DegOmega, tmp, num1, num2, den, DegLambda;

    //form the syndromes; i.e., evaluate HB(x) at roots of g(x)

    for (i = 0; i <= nroots - 1; i++)
    {
        S[i] = HB[0];
    }

    for (j = 1; j <= 62; j++)
    {
        for (i = 0; i <= nroots - 1; i++)
        {
            if (S[i] == 0) {
                S[i] = HB[j];
            } else {
                S[i] = HB[j] ^ rsGFexp[(rsGFlog[S[i]] + i + 1) % 63];
            }
        }
    }

    //convert syndromes to index form, checking for nonzero condition

    SynError = 0;

    for (i = 0; i <= nroots - 1; i++)
    {
        SynError = SynError | S[i];
        S[i] = rsGFlog[S[i]];
    }

    if (SynError == 0)
    {
        //if syndrome is zero, rsData[] is a codeword and there are
        //no errors to correct. So return rsData[] unmodified
        count = 0;
        goto rsDecFinish;
    }

    for (i = 1; i <= nroots; i++)
    {
        lambda[i] = 0;
    }

    lambda[0] = 1;

    for (i = 0; i <= nroots; i++)
    {
        b[i] = rsGFlog[lambda[i]];
    }

    //begin Berlekamp-Massey algorithm to determine error+erasure
    //locator polynomial

    r = 0;
    el = 0;
    while ( r < nroots)  //r is the step number
    {
        r = r + 1;
        //compute discrepancy at the r-th step in poly-form
        DiscrR = 0;
        for (i = 0; i <= r - 1; i++)
        {
            if ((lambda[i] != 0) && (S[r - i - 1] != 63)) {
                DiscrR = DiscrR ^ rsGFexp[(rsGFlog[lambda[i]] + S[r - i - 1]) % 63];
            }
        }
        DiscrR = rsGFlog[DiscrR];  //index form

        if (DiscrR == 63)
        {
            //shift elements upward one step
            for (i = nroots; i >= 1; i += -1)
            {
                b[i] = b[i - 1];
            }
            b[0] = 63;
        } else {
            //t(x) <-- lambda(x) - DiscrR*x*b(x)
            t[0] = lambda[0];
            for (i = 0; i <= nroots - 1; i++)
            {
                if (b[i] != 63) {
                    t[i + 1] = lambda[i + 1] ^ rsGFexp[(DiscrR + b[i]) % 63];
                } else {
                    t[i + 1] = lambda[i + 1];
                }
            }
            if (2 * el <= r - 1)
            {
                el = r - el;
                //b(x) <-- inv(DiscrR) * lambda(x)
                for (i = 0; i <= nroots; i++)
                {
                    if (lambda[i])
                    {
                        b[i] = (rsGFlog[lambda[i]] - DiscrR + 63) % 63;
                    } else {
                        b[i] = 63;
                    }
                }
            } else {
                //shift elements upward one step
                for (i = nroots; i >= 1; i += -1)
                {
                    b[i] = b[i - 1];
                }
                b[0] = 63;
            }
            for (i = 0; i <= nroots; i++)
            {
                lambda[i] = t[i];
            }
        }
    }   /* end while() */

    //convert lambda to index form and compute deg(lambda(x))

    DegLambda = 0;
    for (i = 0; i <= nroots; i++)
    {
        lambda[i] = rsGFlog[lambda[i]];
        if (lambda[i] != 63)
        {
            DegLambda = i;
        }
    }

    //Find roots of the error+erasure locator polynomial by Chien search

    for (i = 1; i <= nroots; i++)
    {
        reg[i] = lambda[i];
    }

    count = 0 ;//number of roots of lambda(x)
    for (i = 1; i <= 63; i++)
    {
        q = 1 ;//lambda[0] is always 0
        for (j = DegLambda; j >= 1; j += -1)
        {
            if (reg[j] != 63)
            {
                reg[j] = (reg[j] + j) % 63;
                q = q ^ rsGFexp[reg[j]];
            }
        }
        if (q == 0)  //it is a root
        {
            //store root (index-form) and error location number
            root[count] = i;
            locn[count] = i - 1;
            //if wehave max possible roots, abort search to save time
            count = count + 1;
            if (count == DegLambda)
            {
                break;
            }
        }
    }

    if (DegLambda != count)
    {
        //deg(lambda) unequal to number of roots => uncorrectable error detected
        count = -1;
        goto rsDecFinish;
    }

    //compute err+eras evaluator poly omega(x)
    // = s(x)*lambda(x) (modulo x**nroots). in index form. Also find deg(omega).

    DegOmega = 0;
    for (i = 0; i <= nroots - 1; i++)
    {
        tmp = 0;
        if (DegLambda < i) {
            j = DegLambda;
        } else {
            j = i;
        }

        for ( /* j = j */ ; j >= 0; j += -1)
        {
            if ((S[i - j] != 63) && (lambda[j] != 63)) {
                tmp = tmp ^ rsGFexp[(S[i - j] + lambda[j]) % 63];
            }
        }
        if (tmp)
        {
            DegOmega = i;
        }
        omega[i] = rsGFlog[tmp];
    }
    omega[nroots] = 63;


    //compute error values in poly-form:
    // num1 = omega(inv(X(l)))
    // num2 = inv(X(l))**(FCR - 1)
    // den = lambda_pr(inv(X(l)))

    for (j = count - 1; j >= 0; j += -1)
    {
        num1 = 0;
        for (i = DegOmega; i >= 0; i += -1)
        {
            if (omega[i] != 63) {
                num1 = num1 ^ rsGFexp[(omega[i] + i * root[j]) % 63];
            }
        }
        num2 = rsGFexp[0];
        den = 0;

        // lambda[i+1] for i even is the formal derivative lambda_pr of lambda[i]
        if (DegLambda < nroots)
        {
            i = DegLambda;
        } else {
            i = nroots;
        }

        for (i = i & ~1; i >= 0; i += -2)
        {
            if (lambda[i + 1] != 63) {
                den = den ^ rsGFexp[(lambda[i + 1] + i * root[j]) % 63];
            }
        }

        if (den == 0)
        {
            count = -1;
            goto rsDecFinish;
        }
 
        // apply error to data
        if (num1 != 0)
        {
            if (locn[j] < FirstInfo) { count = -1; goto rsDecFinish ; } //added by me 
            HB[locn[j]] = HB[locn[j]] ^ (rsGFexp[(rsGFlog[num1] + rsGFlog[num2] + 63 - rsGFlog[den]) % 63]);
        }
    }

    rsDecFinish:
    return (count);
}

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

uint32_t gly23127GetSyn (uint32_t pattern)
{
    uint32_t aux = 0x400000;

    while(pattern & 0xFFFFF800)
    {
        while ((aux & pattern) == 0)
        {
            aux = aux >> 1;
        }
        pattern = pattern ^ (aux / 0x800 * 0xC75);  //generator is C75
    }

    return pattern;
}

uint32_t gly24128Dec (uint32_t n)
{
    //based on gly23127Dec

    uint32_t CW = n >> 1;  //toss the parity bit
    uint32_t correction = gly23127DecTbl[gly23127GetSyn(CW)];
    CW = (CW ^ correction) >> 11;

    return CW;
}

uint32_t gly23127Dec (uint32_t CW)
{
    uint32_t correction = gly23127DecTbl[gly23127GetSyn(CW)];
    CW = (CW ^ correction) >> 11;
    return CW;
}

void ProcHDU(const_bit_vector A)
{
    int i, j, k, ec;
    uint8_t HB[63];   // "hexbit" array

    //header code word is 324 dibits (padded by 5 trailing zero dibits)
    // 324 dibits = 648 bits = 36 18-bit Golay codewords

    //do (18,6,8) shortened Golay decode - make 36 hexbits for rs dec
    for (i = 0; i <= 26; i++) {
        HB[i] = 0;
    }

    k = 0;
    for (i = 0; i < 36; i ++)  // 36 codewords
    {
        uint32_t CW = 0;
        for (j = 0; j < 18; j++) {  // 18 bits / cw
            CW = (CW << 1) + A [ hdu_codeword_bits[k++] ];
        }
        HB[27 + i] = gly24128Dec(CW) & 63;
    }

    //do (36,20,17) RS decode
    ec = rsDec(16, 27, HB);
    //120 info bits = 20 hexbits:   (27..46)
    // 72 bits MI: (27..38)
    //  8 bits MFID
    //  8 bits ALGID
    // 16 bits KID
    // 16 bits TGID

    uint32_t MFID = HB[39] * 4 + (HB[40] >> 4);
    uint32_t ALGID = (HB[40] & 15) * 16 + (HB[41] >> 2);
    uint32_t KID = (HB[41] & 3) * 16384 + HB[42] * 256 + HB[43] * 4 + (HB[44] >> 4);
    uint32_t TGID = (HB[44] & 15) * 4096 + HB[45] * 64 + HB[46];

    fprintf (stderr, " HDU: rc %d mfid %x alg %x kid %x tgid %d", ec, MFID, ALGID, KID, TGID);
}

void ProcLLDU(const_bit_vector A, uint8_t HB[])
{
    int i, j, k;
    for (i = 0; i <= 38; i++) {
        HB[i] = 0;
    }

    k = 0;
    for (i = 0; i < 24; i ++)  // 24 10-bit codewords
    {
        uint32_t CW = 0;
        for (j = 0; j < 10; j++) {  // 10 bits / cw
            CW = (CW << 1) + A [ imbe_ldu_ls_data_bits[k++] ];
        }
        HB[39 + i] = hmg1063Dec( CW >> 4, CW & 0xF );
    }
}

void ProcLC(uint8_t HB[])
{
    int ec = rsDec(12, 39, HB);
    int pb = HB[39] >> 5;
    int sf = (HB[39] & 16) >> 4;
    int lco = (HB[39] & 15) * 4 + (HB[40] >> 4);
    fprintf(stderr, " LC: rc %d pb %d sf %d lco %d", ec, pb, sf, lco);
}

void ProcLDU1(const_bit_vector A)
{
    uint8_t HB[63];   // "hexbit" array

    ProcLLDU(A, HB);
    ProcLC(HB);
}

void ProcLDU2(const_bit_vector A)
{
    uint8_t HB[63];   // "hexbit" array

    ProcLLDU(A, HB);
    int ec = rsDec(8, 39, HB);

    uint32_t ALGID = HB[51] * 4 + (HB[52] >> 4);
    uint32_t KID = (HB[52] & 15) * 4096 + HB[53] * 64 + HB[54];

    fprintf(stderr, " LDU2: rc %d ALGID %x KID %x MI ", ec, ALGID, KID);
    for (int i = 39; i <= 50; i++)
    {
        fprintf(stderr, "%02x ", HB[ i ]);
    }
    // fprintf(stderr, "\n");
}

void ProcTDU(const_bit_vector A)
{
    uint8_t HB[63];   // "hexbit" array
    int i, j, k;

    for (i = 0; i <= 38; i++)
    {
        HB[i] = 0;
    }

    k = 0;
    for (i = 0; i <= 22; i += 2)
    {
        uint32_t CW = 0;
        for (j = 0; j < 12; j++)  // 12 24-bit codewords
        {
            CW = (CW << 1) + A [ hdu_codeword_bits[k++] ];
            CW = (CW << 1) + A [ hdu_codeword_bits[k++] ];
        }
        uint32_t D = gly24128Dec(CW);
        HB[39 + i] = D >> 6;
        HB[40 + i] = D & 63;
    }
    ProcLC(HB);
}

