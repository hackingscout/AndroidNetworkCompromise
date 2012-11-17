package pl.etakt.mobile.networkcompromise;

public class SquaredWrapper {
	// Declare native method (and make it public to expose it directly)
    public static native int squared(int base);
 
    // Provide additional functionality, that "extends" the native method
    public static int to4(int base)
    {
        int sq = squared(base);
        return squared(sq);
    }
 
    // Load library
    static {
        System.loadLibrary("squared");
    }
}
