import argparse
import jinja2
import JsonDBParser

def main():
    arg_parser = argparse.ArgumentParser(description = 'StoredProcedure Generator')
    arg_parser.add_argument('--path', type=str, default='C:\Code\GameServer\Server\Resource\DB\GameDB.json', help='json Path')
    arg_parser.add_argument('--output', type=str, default='GenProcedures.h', help='Output File')
    args = arg_parser.parse_args()

    if args.path == None or args.output == None:
        print('[Error] --path --output required')
        return

    parser = JsonDBParser.JsonDBParser()
    parser.jsonparser(args.path)

    file_loader = jinja2.FileSystemLoader('Templates')
    env = jinja2.Environment(loader=file_loader)
    template = env.get_template('GenProcedures.h')

    output = template.render(procs=parser.procedures)
    f = open(args.output, 'w+')
    f.write(output)
    f.close()

    print(output)

if __name__ == '__main__':
    main()