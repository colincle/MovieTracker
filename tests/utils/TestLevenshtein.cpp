#include <QTest>

#include "Levenshtein.hpp"

class TestLevenshtein : public QObject
{
	Q_OBJECT

  private slots:

	// ── levenshtein() edit distance ──────────────────────────────────────────

	void identicalStringsHaveZeroDistance()
	{
		QCOMPARE(levenshtein("batman", "batman"), 0);
	}

	void bothEmptyHaveZeroDistance() { QCOMPARE(levenshtein("", ""), 0); }

	void emptyVsNonEmptyEqualsLength()
	{
		QCOMPARE(levenshtein("", "abc"), 3);
		QCOMPARE(levenshtein("abc", ""), 3);
	}

	void singleSubstitutionIsOne() { QCOMPARE(levenshtein("cat", "cot"), 1); }

	void singleInsertionIsOne() { QCOMPARE(levenshtein("cat", "cats"), 1); }

	void singleDeletionIsOne() { QCOMPARE(levenshtein("cats", "cat"), 1); }

	void classicKittenSittingIsThree() { QCOMPARE(levenshtein("kitten", "sitting"), 3); }

	void transpositionCostsTwo()
	{
		// Plain Levenshtein (not Damerau): a swap is a delete + insert.
		QCOMPARE(levenshtein("ab", "ba"), 2);
	}

	void isCaseSensitive() { QCOMPARE(levenshtein("Cat", "cat"), 1); }

	void isSymmetric()
	{
		QCOMPARE(levenshtein("flaw", "lawn"), levenshtein("lawn", "flaw"));
	}

	// ── levenshteinThreshold() ───────────────────────────────────────────────

	void thresholdIsZeroForShortQueries()
	{
		QCOMPARE(levenshteinThreshold(""), 0);
		QCOMPARE(levenshteinThreshold("abcd"), 0); // length 4 boundary
	}

	void thresholdIsOneForMediumQueries()
	{
		QCOMPARE(levenshteinThreshold("abcde"), 1);   // length 5
		QCOMPARE(levenshteinThreshold("abcdefg"), 1); // length 7 boundary
	}

	void thresholdIsTwoForLongQueries()
	{
		QCOMPARE(levenshteinThreshold("abcdefgh"), 2); // length 8
		QCOMPARE(levenshteinThreshold("a very long query here"), 2);
	}

	// ── levenshteinMatches() ─────────────────────────────────────────────────

	void shortQueryNeverMatches()
	{
		// Threshold 0 short-circuits to false even on an exact match.
		QVERIFY(!levenshteinMatches("cat", "cat"));
	}

	void exactWordWithinThresholdMatches()
	{
		QVERIFY(levenshteinMatches("hello", "hello world"));
	}

	void closeWordWithinThresholdMatches()
	{
		// "hellp" vs "hello" is distance 1, threshold for length 5 is 1.
		QVERIFY(levenshteinMatches("hellp", "hello world"));
	}

	void matchesAgainstAnyWordInField()
	{
		// "xorld" vs "world" is distance 1.
		QVERIFY(levenshteinMatches("xorld", "hello world"));
	}

	void wordBeyondThresholdDoesNotMatch()
	{
		QVERIFY(!levenshteinMatches("aaaaa", "zzzzz planet"));
	}

	void matchingIsCaseInsensitive()
	{
		QVERIFY(levenshteinMatches("HELLO", "hello world"));
	}

	void emptyFieldDoesNotMatch() { QVERIFY(!levenshteinMatches("hello", "")); }
};

#include "TestLevenshtein.moc"

QObject *createTestLevenshtein()
{
	return new TestLevenshtein();
}
