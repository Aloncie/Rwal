#include "env_utils.hpp"
#include "logs/logs.hpp"
#include <QProcess>
#include <QString>
#include <QDebug>

namespace rwal::platform{
	namespace executor{
		void open_editor(fs::path& path){
			QString editor = QString::fromLocal8Bit(getenv("EDITOR"));
			if (editor.isEmpty())
				editor = QStringLiteral("nano");
			QStringList args;
			args << QString::fromStdString(path.string());

			int exitCode = QProcess::execute(editor, args);
			if (exitCode != 0) {
				qDebug() << "Editor exited with code" << exitCode;
				Logs::getInstance().write_logs("Editor exited with code" + std::to_string(exitCode));
			}			
		}
	}
}
