import argparse
import jinja2
import ProtoParser

def main():
    arg_parser = argparse.ArgumentParser(description = 'PacketGenerator')
    arg_parser.add_argument('--path' ,type=str , default='C:/Code/C++\Servers/Server/Common/Protobuf/bin/Protocol.proto' , help='proto path')
    arg_parser.add_argument('--output' ,type=str , default='GeneratorPacket' , help='output file')
    arg_parser.add_argument('--recv' ,type=str , default='RECV_' , help='recv convention')
    arg_parser.add_argument('--send' ,type=str , default='SEND_' , help='send convention')
    args = arg_parser.parse_args()

    parser = ProtoParser.ProtoParser(1000 , args.recv , args.send)
    parser.parse_proto(args.path)

    file_loader = jinja2.FileSystemLoader('templates')
    env = jinja2.Environment(loader = file_loader)

    template = env.get_template('AutoPacket.h')
    output = template.render(parser = parser , output = args.output)

    f = open(args.output+'.h' , 'w+')
    f.write(output)
    f.close

    print(output)
    return

if __name__ == '__main__':
    main()