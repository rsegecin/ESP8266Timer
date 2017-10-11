#include "SerialInterpreter.h"

void SerialInterpreterClass::ClearBuffer(void)
{
	MessageReady = false;
	ExecFunction = nullptr;
}

char* SerialInterpreterClass::GetParameter(unsigned char index)
{
	uint8_t num;
	uint8_t parameters = 0;
	char *ret_aux = NULL;

	for (num = 0; num < DEF_MSG_SIZE; num++)
	{
		if (StrParameters[num] != 0x00)
		{
			if (parameters == index)
			{
				ret_aux = &StrParameters[num];
			}

			while ((StrParameters[num] != 0x00) && (num < DEF_MSG_SIZE)) { num++; }

			parameters++;
		}
	}

	return ret_aux;
}

void SerialInterpreterClass::OnInterrupt(char charParam)
{
	static bool flagD = false;

	static char estado = 0;
	static bool emEspera;

	static uint8_t matchingCmd;
	char * strMatchingCmd;
	static uint8_t msgPosition;
	static uint8_t msgCmdPosition;

	static uint8_t countChar = 0;
	bool tmpBit;
	uint8_t num;
	char * strCmd;

	if (MessageReady == false)
	{
		MessageBuffer[msgPosition] = charParam;

		switch (estado)
		{
		case 0:
			//valores das variáveis para entrar no estado 0 (preseted)
			emEspera = false;
			matchingCmd = 0;
			msgPosition = 0;
			msgCmdPosition = 0;

			if (charParam == 0x0D)
				flagD = true;
			else if ((flagD == true) && (charParam == 0x0A))
			{
				memset(MessageBuffer, 0x00, DEF_MSG_SIZE);
				estado = 1;
				flagD = false;
			}
			else
				flagD = false;
			break;
		case 1:
			strMatchingCmd = (char *)SerialCommands[matchingCmd].Name;

			if (emEspera == false)
			{
				num = isalnum(charParam);

				if (num != 0)
				{
					if (charParam == strMatchingCmd[msgCmdPosition])
					{
						msgCmdPosition++;
					}
					else
					{
						//Verifica parte do comando
						tmpBit = false;

						for (num = matchingCmd + 1; num < NumberOfCommands; num++)
						{
							strCmd = (char *)SerialCommands[num].Name;
							if ((strncmp(strMatchingCmd, strCmd, msgCmdPosition) == 0) && (charParam == strCmd[msgCmdPosition]))
							{
								matchingCmd = num;
								tmpBit = true;
								break;
							}
						}

						if (tmpBit == true)
						{
							//tem comando
							msgCmdPosition++;
						}
						else
						{
							//nenhum bateu
							emEspera = true;
							countChar = 0;
							matchingCmd = 0;
							msgCmdPosition = 0;
							num = 0;
						}
					}
				}
				else
				{
					if ((charParam == ',') || (charParam == ';') || (charParam == '='))
					{
						if (msgCmdPosition == strlen(SerialCommands[matchingCmd].Name))
						{
							estado = 2;
							msgCmdPosition = 0;
						}
						else
						{
							//Verifica todo o comando
							tmpBit = false;

							for (num = matchingCmd + 1; num < NumberOfCommands; num++)
							{
								strCmd = (char *)SerialCommands[num].Name;
								if ((strncmp(strMatchingCmd, strCmd, msgCmdPosition) == 0) && (strlen(strCmd) == msgCmdPosition))
								{
									matchingCmd = num;
									tmpBit = true;
									break;
								}
							}

							if (tmpBit == true)
							{
								//tem comando
								estado = 2;
								msgCmdPosition = 0;
							}
							else
							{
								//nenhum bateu, deve continuar no mesmo estado pois recebeu caractere de separação
								matchingCmd = 0;
								msgCmdPosition = 0;
							}
						}
					}
					else
					{
						emEspera = true;
						countChar = 0;
						num = 0;

						if (charParam == 0x0D)
							flagD = true;
					}
				}
			}
			else
			{ // Else do estado emEspera
				if ((charParam == ',') || (charParam == ';') || (charParam == '='))
				{
					emEspera = false;
					matchingCmd = 0;
					msgCmdPosition = 0;
				}
				else
				{
					if (charParam == 0x0D)
					{
						flagD = true;
					}
					else if ((flagD == true) && (charParam == 0x0A))
					{
						if (msgPosition > 2)
						{
							if (countChar == 0)
							{
								//Verifica todo o comando
								tmpBit = false;

								for (num = matchingCmd; num < NumberOfCommands; num++)
								{
									strCmd = (char *)SerialCommands[num].Name;
									if ((strncmp(strMatchingCmd, strCmd, msgCmdPosition) == 0) && (strlen(strCmd) == msgCmdPosition))
									{
										matchingCmd = num;
										tmpBit = true;
										break;
									}
								}

								if (tmpBit == true)
								{
									ExecFunction = SerialCommands[matchingCmd].ExecFunction;
									MessageReady = true;
								}
								else
								{
									ExecFunction = nullptr;
								}
							}
							else
							{
								ExecFunction = nullptr;
							}

							estado = 0;
						}

						strCmd = MessageBuffer + 1;
						memmove(MessageBuffer, strCmd, strlen(strCmd));
						memset(MessageBuffer + (msgPosition - 2), 0x00, DEF_MSG_SIZE - (msgPosition - 2));

						matchingCmd = 0;
						msgCmdPosition = 0;
						emEspera = false;
						flagD = false;
					}
					else
					{
						flagD = false;
					}
				}
				countChar++;
			}
			break;
		case 2:
			if ((charParam == ',') || (charParam == ';') || (charParam == '='))
			{
				StrParameters[msgCmdPosition] = 0x00;
				msgCmdPosition++;
			}
			else if (charParam == 0x0D)
			{
				StrParameters[msgCmdPosition] = charParam;
				msgCmdPosition++;
				flagD = true;
			}
			else if ((flagD == true) && (charParam == 0x0A))
			{
				flagD = false;
				StrParameters[msgCmdPosition - 1] = 0x00;

				strCmd = MessageBuffer + 1;
				memmove(MessageBuffer, strCmd, strlen(strCmd));
				memset(MessageBuffer + (msgPosition - 2), 0x00, DEF_MSG_SIZE - (msgPosition - 2));

				MessageReady = true;
				ExecFunction = SerialCommands[matchingCmd].ExecFunction;

				matchingCmd = 0;
				msgCmdPosition = 0;
				emEspera = false;
				estado = 0;
			}
			else
			{
				StrParameters[msgCmdPosition] = charParam;
				msgCmdPosition++;
				flagD = false;
			}
			break;
		}

		msgPosition++;
	}
}

SerialInterpreterClass::SerialInterpreterClass(sSerialCommand * pSerialCommands, int pNumberOfCommands)
{
	SerialCommands = pSerialCommands;
	NumberOfCommands = pNumberOfCommands;
	ClearBuffer();
}