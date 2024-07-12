#ifndef floatsAreEqual_H_
#define floatsAreEqual_H_

#define floatsAreEqual(a, b) \
  (fabs((a) - (b)) <= DBL_EPSILON * fmax(1.0, fmax(fabs(a), fabs(b))))

#endif // floatsAreEqual_H_
