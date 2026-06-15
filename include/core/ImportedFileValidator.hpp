#pragma once

#include <QString>

struct ValidationResult
{
	bool valid = false;
	QString error;
};

class ImportedFileValidator
{
public:
	static ValidationResult validate(const QString &filePath);
};
