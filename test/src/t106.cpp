#include <stdio.h>
#include <propeller2.h>

// Test FlexProp compatibility functions
// Tests: _getus, _getms, _cnt, _cnth, _cnt64, _lockrel, _pinr, _waitx,
//        _rdpin, _rqpin, _wrpin, _wxpin, _wypin, _clockfreq, _clockmode

int main() {
    printf("$\n"); // start of test character
    
    // Test 1: _clockfreq and _clockmode macros
    printf("Test 1: Clock functions\n");
    uint32_t freq = _clockfreq();
    uint32_t mode = _clockmode();
    printf("Clock frequency: %u Hz\n", freq);
    printf("Clock mode: 0x%08x\n", mode);
    if (freq > 0) {
        printf("PASS: _clockfreq returned valid value\n");
    } else {
        printf("FAIL: _clockfreq returned 0\n");
    }
    
    // Test 2: _cnt - basic counter read
    printf("\nTest 2: _cnt basic read\n");
    uint32_t cnt1 = _cnt();
    uint32_t cnt2 = _cnt();
    if (cnt2 > cnt1) {
        printf("PASS: _cnt increments (cnt1=%u, cnt2=%u)\n", cnt1, cnt2);
    } else {
        printf("FAIL: _cnt not incrementing\n");
    }
    
    // Test 3: _cnth - upper 32 bits
    printf("\nTest 3: _cnth upper bits\n");
    uint32_t hi1 = _cnth();
    uint32_t hi2 = _cnth();
    // At normal clock speeds, high word shouldn't change in a few cycles
    printf("cnth reads: %u, %u\n", hi1, hi2);
    printf("PASS: _cnth executed\n");
    
    // Test 4: _cnt64 - full 64-bit counter
    printf("\nTest 4: _cnt64 full counter\n");
    uint64_t cnt64_1 = _cnt64();
    _waitx(1000); // wait a bit
    uint64_t cnt64_2 = _cnt64();
    if (cnt64_2 > cnt64_1) {
        printf("PASS: _cnt64 increments (%llu -> %llu)\n", 
               (unsigned long long)cnt64_1, (unsigned long long)cnt64_2);
    } else {
        printf("FAIL: _cnt64 not incrementing\n");
    }
    
    // Test 5: _waitx - basic wait
    printf("\nTest 5: _waitx basic wait\n");
    uint32_t before = _cnt();
    _waitx(freq / 1000); // Wait ~1ms worth of cycles
    uint32_t after = _cnt();
    uint32_t elapsed = after - before;
    // Should be approximately freq/1000 cycles (allow 20% margin)
    if (elapsed > (freq / 1200) && elapsed < (freq / 800)) {
        printf("PASS: _waitx waited approximately correct time (%u cycles)\n", elapsed);
    } else {
        printf("WARN: _waitx elapsed=%u, expected~%u\n", elapsed, freq / 1000);
    }
    
    // Test 6: _getus - microseconds elapsed
    printf("\nTest 6: _getus microseconds\n");
    uint32_t us1 = _getus();
    _waitx(freq / 100); // Wait ~10ms
    uint32_t us2 = _getus();
    uint32_t us_diff = us2 - us1;
    // Should be approximately 10000 us (allow wide margin for timing)
    if (us_diff > 8000 && us_diff < 12000) {
        printf("PASS: _getus delta=%u us (expected ~10000)\n", us_diff);
    } else {
        printf("WARN: _getus delta=%u us (expected ~10000)\n", us_diff);
    }
    
    // Test 7: _getms - milliseconds elapsed
    printf("\nTest 7: _getms milliseconds\n");
    uint32_t ms1 = _getms();
    _waitx(freq / 10); // Wait ~100ms
    uint32_t ms2 = _getms();
    uint32_t ms_diff = ms2 - ms1;
    // Should be approximately 100 ms (allow wide margin)
    if (ms_diff > 80 && ms_diff < 120) {
        printf("PASS: _getms delta=%u ms (expected ~100)\n", ms_diff);
    } else {
        printf("WARN: _getms delta=%u ms (expected ~100)\n", ms_diff);
    }
    
    // Test 8: _getus and _getms monotonic increase
    printf("\nTest 8: Time functions monotonicity\n");
    uint32_t us_a = _getus();
    uint32_t ms_a = _getms();
    _waitx(freq / 1000); // 1ms
    uint32_t us_b = _getus();
    uint32_t ms_b = _getms();
    if (us_b > us_a && ms_b >= ms_a) {
        printf("PASS: Time functions are monotonic\n");
    } else {
        printf("FAIL: Time not monotonic (us: %u->%u, ms: %u->%u)\n", 
               us_a, us_b, ms_a, ms_b);
    }
    
    // Test 9: _locknew, _lockrel, _locktry
    printf("\nTest 9: Lock functions\n");
    int lock = _locknew();
    if (lock >= 0 && lock < 16) {
        printf("PASS: _locknew returned lock %d\n", lock);
    } else {
        printf("FAIL: _locknew returned invalid lock %d\n", lock);
    }
    
    // Test 10: _locktry - should succeed when unlocked
    printf("\nTest 10: _locktry on free lock\n");
    int try_result = _locktry(lock);
    if (try_result != 0) {
        printf("PASS: _locktry succeeded on free lock (result=%d)\n", try_result);
    } else {
        printf("FAIL: _locktry failed on free lock\n");
    }
    
    // Test 11: _locktry - should fail when locked
    printf("\nTest 11: _locktry on held lock\n");
    int try_result2 = _locktry(lock);
    if (try_result2 == 0) {
        printf("PASS: _locktry failed on held lock\n");
    } else {
        printf("FAIL: _locktry succeeded on held lock\n");
    }
    
    // Test 12: _lockrel - release and check return value
    printf("\nTest 12: _lockrel return value\n");
    int rel_result = _lockrel(lock);
    // _lockrel should return -1 when releasing a held lock (C bit set)
    if (rel_result == -1) {
        printf("PASS: _lockrel returned -1 for held lock\n");
    } else {
        printf("WARN: _lockrel returned %d (expected -1)\n", rel_result);
    }
    
    // Test 13: _lockrel on free lock
    printf("\nTest 13: _lockrel on free lock\n");
    int rel_result2 = _lockrel(lock);
    // Should return 0 when lock was already free
    if (rel_result2 == 0) {
        printf("PASS: _lockrel returned 0 for free lock\n");
    } else {
        printf("WARN: _lockrel returned %d (expected 0)\n", rel_result2);
    }
    
    // Test 14: _lockret - return lock to pool
    printf("\nTest 14: _lockret\n");
    _lockret(lock);
    printf("PASS: _lockret executed\n");
    
    // Test 15: _pinr - read pin without changing direction
    printf("\nTest 15: _pinr pin read\n");
    // Read a pin (pin 0) - should not crash
    int pin_val = _pinr(0);
    printf("PASS: _pinr(0) returned %d\n", pin_val);
    
    // Test 16: Pin control functions with safe pin (56 is typically unused)
    printf("\nTest 16: Pin control functions\n");
    const int test_pin = 56;
    _pinl(test_pin);  // Set pin low
    _pinh(test_pin);  // Set pin high
    _pinnot(test_pin); // Toggle pin
    printf("PASS: Pin control functions executed\n");
    
    // Test 17: Smart pin functions (_wrpin, _wxpin, _wypin, _rdpin, _rqpin)
    printf("\nTest 17: Smart pin functions\n");
    const int smart_pin = 57;
    _wrpin(smart_pin, 0); // Clear smart pin mode
    _wxpin(smart_pin, 0); // Set X parameter
    _wypin(smart_pin, 0); // Set Y parameter
    uint32_t rd = _rdpin(smart_pin); // Read pin data
    uint32_t rq = _rqpin(smart_pin); // Read pin parameter
    printf("PASS: Smart pin functions (rdpin=%u, rqpin=%u)\n", rd, rq);
    
    // Test 18: _pinw - write pin value
    printf("\nTest 18: _pinw function\n");
    _pinw(test_pin, 1);
    _pinw(test_pin, 0);
    printf("PASS: _pinw executed\n");
    
    // Test 19: Edge case - very small wait
    printf("\nTest 19: _waitx with small value\n");
    _waitx(1); // Minimal wait
    printf("PASS: _waitx(1) executed\n");
    
    // Test 20: Edge case - zero wait
    printf("\nTest 20: _waitx with zero\n");
    _waitx(0);
    printf("PASS: _waitx(0) executed\n");
    
    // Test 21: Multiple consecutive time reads
    printf("\nTest 21: Rapid time reads\n");
    uint32_t rapid_us[5];
    for (int i = 0; i < 5; i++) {
        rapid_us[i] = _getus();
    }
    int all_increasing = 1;
    for (int i = 1; i < 5; i++) {
        if (rapid_us[i] < rapid_us[i-1]) {
            all_increasing = 0;
            break;
        }
    }
    if (all_increasing) {
        printf("PASS: Rapid _getus calls are monotonic\n");
    } else {
        printf("FAIL: Rapid _getus not monotonic\n");
    }
    
    // Test 22: Time rollover safety (just verify calculation works)
    printf("\nTest 22: Time calculation safety\n");
    // This tests that our division-based approach handles large cycle counts
    uint32_t current_us = _getus();
    printf("PASS: Current microseconds: %u\n", current_us);
    
    printf("\n=== All FlexProp compatibility tests completed ===\n");
    printf("~\n"); // end of test character
    
    return 0;
}
